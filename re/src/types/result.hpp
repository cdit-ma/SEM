//
// Created by Jackson Michael on 20/8/20.
// With reference from https://en.cppreference.com/w/cpp/utility/variant/visit
//

#ifndef SEM_RESULT_HPP
#define SEM_RESULT_HPP

#include <string>
#include <iostream>
#include <optional>
#include <variant>
#include <stdexcept>

namespace sem {

// helper constant for the visitor #3
    template<class> inline constexpr bool always_false_v = false;

    template<class... Ts>
    struct overloaded : Ts ... {
        using Ts::operator()...;
    };
// explicit deduction guide (not needed as of C++20)
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;


    struct ErrorResult {
        explicit ErrorResult(std::string error_message) : msg(std::move(error_message)) {};
        std::string msg;

        bool operator==(const ErrorResult &other) const {
            return msg == other.msg;
        };

        friend std::ostream &operator<<(std::ostream &os, const ErrorResult &err) {
            return os << "ErrorResult: {'" << err.msg << "'}";
        };
    };

/*std::ostream& operator<<(std::ostream& os, const ErrorResult& er)
{
    return os << er;
};*/


    class unexpected_error_result : public std::exception {
    public:
        explicit unexpected_error_result(std::string message) : msg_(std::move(message)) {};

        [[nodiscard]] const char *what() const noexcept override {
            return msg_.c_str();
        }

    private:
        std::string msg_;
    };

    class unexpected_value_result : public std::exception {
    public:
        explicit unexpected_value_result(std::string message) : msg_(std::move(message)) {};

        [[nodiscard]] const char *what() const noexcept override {
            return msg_.c_str();
        }

    private:
        std::string msg_;
    };


    template<typename ValueType>
    struct Result {
        using val_or_err_t = std::variant<ValueType, ErrorResult>;

#ifdef _WIN32
        [[deprecated("Default constructor only exists for compatibility with MSVC std::future")]]
        constexpr Result() : value_(ErrorResult("Result type created without a value")){};
#endif

        constexpr Result(val_or_err_t value) : value_(std::move(value)) {};

        Result(ErrorResult errorResult) : Result(val_or_err_t(std::move(errorResult))) {};

        constexpr Result(ValueType value) : Result(val_or_err_t(std::move(value))) {};

        // NOTE: rvalue ref constructors should only be enabled if the provided value is an rvalue expression
        // see: https://stackoverflow.com/questions/16999039/why-are-my-t-and-t-copy-constructors-ambiguous
        template <typename std::enable_if<!std::is_reference_v<ValueType>>::type* = nullptr>
        constexpr Result(val_or_err_t&& value) : value_(value) {};
        template <typename std::enable_if<!std::is_reference_v<ValueType>>::type* = nullptr>
        constexpr Result(ValueType&& value) : Result(val_or_err_t(value)) {};

        [[nodiscard]]
        constexpr ValueType& GetValue() {
            return std::visit(
                    [](auto &&value) -> ValueType& {
                        using T = std::decay_t<decltype(value)>;
                        if constexpr (std::is_same_v<T, ValueType>) {
                            return value;
                        } else if constexpr (std::is_same_v<T, ErrorResult>) {
                            throw unexpected_error_result(value.msg);
                        } else {
                            static_assert(always_false_v<T>, "non-exhaustive visitor!");
                        }
                    }, value_);
        };

        [[nodiscard]]
        constexpr ValueType GetValue() const {
            return std::visit(
                    [](auto &&value) -> ValueType {
                        using T = std::decay_t<decltype(value)>;
                        if constexpr (std::is_same_v<T, ValueType>) {
                            return value;
                        } else if constexpr (std::is_same_v<T, ErrorResult>) {
                            throw unexpected_error_result(value.msg);
                        } else {
                            static_assert(always_false_v<T>, "non-exhaustive visitor!");
                        }
                    }, value_);
        };

        [[nodiscard]]
        ErrorResult GetError() const {
            return std::visit(
                    [](auto &&value) -> ErrorResult {
                        using T = std::decay_t<decltype(value)>;
                        if constexpr (std::is_same_v<T, ValueType>) {
                            throw unexpected_value_result(std::string("Result contains Value instead of ErrorResult: ")
                                                          + typeid(ValueType).name());
                        } else if constexpr (std::is_same_v<T, ErrorResult>) {
                            return value;
                        } else {
                            static_assert(always_false_v<T>, "non-exhaustive visitor!");
                        }
                    }, value_);
        };

        [[nodiscard]]
        constexpr bool is_error() const {
            return std::visit(
                    [](auto &&value) -> bool {
                        using T = std::decay_t<decltype(value)>;
                        return std::is_same_v<T, ErrorResult>;
                    }, value_);
        }

        bool operator==(const Result &other) {
            return value_ == other.value_;
        };

        friend std::ostream &operator<<(std::ostream &os, const Result<ValueType> &res) {
            if (res.is_error()) {
                return os << "Result: {" << res.GetError() << "}";
            } else {
                return os << "Result: {" << res.GetValue() << "}";
            }
        };

    private:
        val_or_err_t value_;
    };

    template<>
    struct Result<void> {
        Result() : err_(std::nullopt) {};

        Result(ErrorResult error) : err_(std::move(error)) {};

        void GetValue() const {
            if (err_) {
                throw unexpected_error_result(err_->msg);
            }
        };

        [[nodiscard]]
        ErrorResult GetError() const {
            if (!err_) {
                throw unexpected_value_result("Void value");
            }
            return *err_;
        };

        [[nodiscard]]
        constexpr bool is_error() const {
            return err_.has_value();
        }

        bool operator==(const Result<void> &other) const {
            return err_ == other.err_;
        };

        friend std::ostream &operator<<(std::ostream &os, const Result<void> &res) {
            if (res.err_) {
                return os << "Result: {" << *res.err_ << "}";
            } else {
                return os << "Result: {void}";
            }
        };
    private:
        const std::optional<ErrorResult> err_;
    };

    template<typename LH_ValueType, typename RH_ValueType>
    inline auto operator==(const Result<LH_ValueType> &lh, const Result<RH_ValueType> &rh) {
        return lh == rh;
    }

}
#endif //SEM_RESULT_HPP
