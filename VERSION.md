# Runtime Environment v3.3.6
* Squashed many build warnings
* Added jenkins script to rebuild docker images from a custom base image: `rebuild_docker_images`
* deploy_model jenkins script now builds binaries to be deployed to a docker container against the same docker container
* Fix oversight in memory worker test
* Many updates to dependency install instructions
* Moved to more modern CMake usage, allowing for future use of cpp14 and cpp17 features

## Resolved Jira Tickets:
* RE-449

