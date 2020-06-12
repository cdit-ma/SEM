# MEDEA v3.4.2

## Miscellaneous
* Bug fixes and prep work for moving to the new SEM monorepo

## New Features (from v3.4.1):
* Implemented basic Trigger/Strategy paradigm for allowing experiments to respond to hardware events.
    * Added a Trigger Browser widget for the construction of the Triggers
    * Added a the following new entities:
        * Trigger
        * TriggerInstance
        * StrategyInstance  
    * Added the capability to add TriggerInstances inside ComponentInstances
    * Added the capability to connect StrategyInstances to DeploymentContainers
    
* Implemented new CUDA Worker to provide multi-gpu compute capabilities
    * Currently performs FFT computations that are automatically distributed across all GPUs
    * Intended to demonstrate base capability and will be extended in the future.