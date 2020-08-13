#include "versionkey.h"
#include "../data.h"
#include "../node.h"
#include <QDebug>
#include <QRegularExpression>
#include <stdexcept>

const QRegularExpression VERSION_REGEX{R"(^(\d+)\.(\d+)\.(\d+)(-.+)?$)"};


VersionKey::VersionKey(EntityFactoryBroker& broker): Key(broker, KeyName::Version, QVariant::String)
{
}

QVariant VersionKey::validateDataChange(Data* data, QVariant data_value){
    auto value = data_value.toString();

    if(!IsVersionValid(value)){
        value = GetDefaultVersion();
    }
    return Key::validateDataChange(data, value);
}

// Throws std::invalid_argument when passed version string that does not match regex:
// eg 12.1.11-a
// eg 1.2.0-b
// eg 0.1.16-test
// See: https://semver.org/ for full specification (we allow an arbitrary string on the end)
int VersionKey::CompareVersion(const QString& current_version, const QString& compare_version){
    const auto current_match = VERSION_REGEX.match(current_version);
    const auto compare_match = VERSION_REGEX.match(compare_version);
    
    if(!current_match.hasMatch()) {
        throw std::invalid_argument("Current Version invalid format");
    }
    
    if(!compare_match.hasMatch()) {
        throw std::invalid_argument("Compare Version invalid format");
    }

    // Iterate over the three numeric fields
    // First matching regex group is start caret, therefore skip and start at group 1
    for(int i = 1; i < 4; i++) {
        int current = current_match.captured(i).toInt();
        int compare = compare_match.captured(i).toInt();

        if(current == compare) {
            // Keep comparing if we have a match
            continue;
        } else if(current > compare) {
            // Bail out early if anything other than a match
            return 1;
        } else if(current < compare) {
            return -1;
        }
    }

    // All numeric version numbers match, compare suffix strings lexicographically
    return current_match.captured(4).compare(compare_match.captured(4), Qt::CaseInsensitive);
}

bool VersionKey::IsVersionOlder(const QString& current_version, const QString& compare_version){
    return CompareVersion(current_version, compare_version) < 0;
}

bool VersionKey::IsVersionNewer(const QString& current_version, const QString& compare_version){
    return CompareVersion(current_version, compare_version) > 0;
}

bool VersionKey::IsVersionValid(const QString& version){
    return VERSION_REGEX.match(version).hasMatch();
}

const QString& VersionKey::GetDefaultVersion(){
    const static QString DEFAULT_VERSION{"0.0.0"};
    return DEFAULT_VERSION;
}