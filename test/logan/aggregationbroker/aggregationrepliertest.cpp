//
// Created by Jackson on 4/02/2020.
//

#include "gtest/gtest.h"

#include <aggregationreplier.h>
#include <docker.h>

#include <thread>

using namespace re::logging::aggregation::broker;

// TODO: Mock one or more DatabaseClients for proper dependency injection

TEST(Re_Logan_Aggregation, AggregationReplier_Construct)
{
    // Create a client for communicating with the docker process
    Docker client{};

    // Look at the images we have locally
    JSON_DOCUMENT image_response = client.list_images();
    std::cout << jsonToString(image_response) << std::endl;
    const auto& image_list = image_response["data"].GetArray();
    const auto& match = std::find_if(image_list.begin(), image_list.end(),
            [](const auto& value) -> bool {
                assert(value.GetObject().HasMember("RepoTags"));
                const auto& repo_tag_list = value.GetObject()["RepoTags"];
                for (const auto& repo_tag : repo_tag_list.GetArray()) {
                    std::cout << jsonToString(repo_tag) << std::endl;
                    if (jsonToString(repo_tag).find("postgres") != std::string::npos) {
                        return true;
                    }
                }
                return false;
    });

    // If we dont have the required postgres image, make it
    if (match == image_list.end()) {
        std::cout << "Postgres:12 image not found locally, will create..." << std::endl;
        JSON_DOCUMENT create_image_response = client.create_image(
                "postgres", "", "", "12");
        std::cout << "images/create response: " << jsonToString(create_image_response) << std::endl;
        ASSERT_TRUE(create_image_response.HasMember("success"));
        // The following lines occasionally cause tests to fail as no "code" member is populated by DockerClient
        //ASSERT_TRUE(create_image_response.HasMember("code"));
        //ASSERT_EQ(create_image_response["code"].Get<int>(), 200);
        ASSERT_TRUE(create_image_response["success"].Get<bool>());
    }

    // Create the JSON document that describes the container we need
    JSON_DOCUMENT postgres_create_params(rapidjson::kObjectType);
    auto& alloc = postgres_create_params.GetAllocator();

    // Currently basing development off of Postgresql version 12
    postgres_create_params.AddMember("Image", "postgres:12", alloc);

    // Set the DB password in our environment variables (Object with keyname "Env" mapping to array of strings)
    rapidjson::Value env_array{rapidjson::kArrayType};
    env_array.PushBack(
            rapidjson::Value{"POSTGRES_PASSWORD=test-p4ssword", alloc}, alloc
            );
    postgres_create_params.AddMember("Env", env_array, alloc);


    // Create a host port representation and add it to the list of host ports we want to map to
    rapidjson::Value host_port(rapidjson::kObjectType);
    host_port.AddMember("HostPort", "5432", alloc);

    rapidjson::Value host_ports{rapidjson::kArrayType};
    host_ports.GetArray().PushBack(
            host_port, alloc
            );

    // Attach the list of host ports to our internally exposed 5432 port
    rapidjson::Value port_bindings((rapidjson::kObjectType));
    port_bindings.AddMember("5432/tcp", host_ports, alloc);

    // Add the entire port binding entity to the host configuration, which can then be added to our params
    rapidjson::Value host_config(rapidjson::kObjectType);
    host_config.AddMember("PortBindings", port_bindings, alloc);
    postgres_create_params.AddMember("HostConfig", host_config, alloc);


    std::cout << "after pushback: " << jsonToString(postgres_create_params) << std::endl;

    auto create_result = client.create_container(postgres_create_params);
    std::cout << jsonToString(create_result) << std::endl;
    ASSERT_TRUE(create_result.HasMember("success"));
    ASSERT_TRUE(create_result["success"].Get<bool>());
    std::cout << jsonToString(create_result["data"].GetObject()["Id"]) << std::endl;
    const auto& pg_container_id = create_result["data"].GetObject()["Id"].Get<std::string>();

    std::cout << "Container created, about to start..." << std::endl;
    auto start_result = client.start_container(pg_container_id);
    std::cout << jsonToString(start_result) << std::endl;
    ASSERT_TRUE(start_result.HasMember("success"));
    ASSERT_TRUE(start_result["success"].Get<bool>());

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    std::shared_ptr<DatabaseClient> db_client;
    std::string db_string("dbname = postgres user = postgres password = test-p4ssword "
    "hostaddr = 0.0.0.0 port = 5432");

    // Create database client
    // TODO: replace with mock version!
    try {
        db_client = std::make_shared<DatabaseClient>(db_string);
    } catch (std::exception& e) {
        std::cout << "Failed to create DatabaseClient: " << e.what() << std::endl;
        GTEST_FAIL();
    }

    // Make our AggregationReplier
    EXPECT_NO_THROW(
        AggregationReplier replier(db_client);
        );

    // Clean up our container
    auto stop_result = client.stop_container(pg_container_id);
    ASSERT_TRUE(stop_result.HasMember("success"));
    ASSERT_TRUE(stop_result["success"].Get<bool>());

    auto delete_result = client.delete_container(pg_container_id);
    ASSERT_TRUE(delete_result.HasMember("success"));
    ASSERT_TRUE(delete_result["success"].Get<bool>());
}