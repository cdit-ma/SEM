-- ****************** SqlDBM: Modified for postgres ******************;
-- ***************************************************;

DROP TABLE IF EXISTS PortLifecycleEvent;


DROP TABLE IF EXISTS WorkloadEvent;


DROP TABLE IF EXISTS PortEvent;


DROP TABLE IF EXISTS ComponentLifecycleEvent;


DROP TABLE IF EXISTS UserEvent;


DROP TABLE IF EXISTS WorkerInstance;


DROP TABLE IF EXISTS Port;


DROP TABLE IF EXISTS ComponentInstance;


DROP TABLE IF EXISTS Hardware.SystemStatus;


DROP TABLE IF EXISTS Hardware.ProcessStatus;


DROP TABLE IF EXISTS Hardware.InterfaceStatus;


DROP TABLE IF EXISTS Hardware.FilesystemStatus;


DROP TABLE IF EXISTS Hardware.System;


DROP TABLE IF EXISTS Hardware.CPUStatus;


DROP TABLE IF EXISTS Hardware.Process;


DROP TABLE IF EXISTS Hardware.Interface;


DROP TABLE IF EXISTS Hardware.Filesystem;


DROP TABLE IF EXISTS Container;


DROP TABLE IF EXISTS Worker;


DROP TABLE IF EXISTS Component;


DROP TABLE IF EXISTS Node;


DROP TABLE IF EXISTS ExperimentRun;


DROP TABLE IF EXISTS Experiment;


DROP SCHEMA IF EXISTS Hardware;


CREATE SCHEMA Hardware;

-- ************************************** Experiment

CREATE TABLE Experiment
(
 Name         TEXT NOT NULL ,
 ExperimentID SERIAL ,
 ModelName    TEXT NOT NULL ,
 Metadata     JSON ,

PRIMARY KEY (ExperimentID),
CONSTRAINT UniqueExperimentName UNIQUE (Name)
);






-- ************************************** ExperimentRun

CREATE TABLE ExperimentRun
(
 ExperimentID    INT NOT NULL ,
 ExperimentRunID SERIAL ,
 JobNum          INT NOT NULL ,
 StartTime       TIMESTAMP NOT NULL ,
 EndTime         TIMESTAMP ,
 LastUpdated     TIMESTAMP ,
 Metadata        JSON ,

PRIMARY KEY (ExperimentRunID),
CONSTRAINT UniqueJobNumPerExperiment UNIQUE (ExperimentID, JobNum),
CONSTRAINT FK_Job_ExperimentID_Experiment_ExperimentID FOREIGN KEY (ExperimentID) REFERENCES Experiment (ExperimentID)
);






-- ************************************** Worker

CREATE TABLE Worker
(
 WorkerID        SERIAL ,
 Name            TEXT NOT NULL ,
 ExperimentRunID INT NOT NULL ,
 GraphmlID       TEXT NOT NULL ,

PRIMARY KEY (WorkerID),
CONSTRAINT UniqueWorkerNamePerRun UNIQUE (ExperimentRunID, Name),
CONSTRAINT FK_420 FOREIGN KEY (ExperimentRunID) REFERENCES ExperimentRun (ExperimentRunID)
);






-- ************************************** Component

CREATE TABLE Component
(
 ComponentID     SERIAL ,
 Name            TEXT NOT NULL ,
 ExperimentRunID INT NOT NULL ,
 GraphmlID       TEXT NOT NULL ,

PRIMARY KEY (ComponentID),
CONSTRAINT UniqueComponentNamePerRun UNIQUE (Name, ExperimentRunID),
CONSTRAINT FK_403 FOREIGN KEY (ExperimentRunID) REFERENCES ExperimentRun (ExperimentRunID)
);






-- ************************************** Node

CREATE TABLE Node
(
 NodeID          SERIAL ,
 Hostname        TEXT NOT NULL ,
 IP              INET NOT NULL ,
 ExperimentRunID INT NOT NULL ,
 GraphmlID       TEXT NOT NULL ,

PRIMARY KEY (NodeID),
CONSTRAINT UniqueIP UNIQUE (IP, ExperimentRunID),
CONSTRAINT FK_360 FOREIGN KEY (ExperimentRunID) REFERENCES ExperimentRun (ExperimentRunID)
);








-- ************************************** Cluster

CREATE TABLE Container
(
 ContainerID       SERIAL ,
 NodeID          INT NOT NULL ,
 Name            TEXT NOT NULL ,
 GraphmlID       TEXT NOT NULL,
 Type            TEXT NOT NULL,

PRIMARY KEY (ContainerID),
CONSTRAINT UniqueGraphmlIDPerRun UNIQUE (NodeID, GraphmlID),
CONSTRAINT FK_356 FOREIGN KEY (NodeID) REFERENCES Node (NodeID)
);






-- ************************************** Hardware.System

CREATE TABLE Hardware.System
(
 SystemID     SERIAL ,
 OSName         TEXT NOT NULL ,
 OSArch         TEXT NOT NULL ,
 OSDescription  TEXT NOT NULL ,
 OSVersion      TEXT NOT NULL ,
 OSVendor       TEXT NOT NULL ,
 OSVendorName   TEXT NOT NULL ,
 CPUModel       TEXT NOT NULL ,
 CPUVendor      TEXT NOT NULL ,
 CPUFrequencyHz   INT NOT NULL ,
 PhysicalMemoryKB BIGINT NOT NULL ,
 NodeID      INT NOT NULL ,

PRIMARY KEY (SystemID),
CONSTRAINT UniqueSystemPerNode UNIQUE (NodeID),
CONSTRAINT FK_373 FOREIGN KEY (NodeID) REFERENCES Node (NodeID)
);






-- ************************************** Hardware.CPUStatus

CREATE TABLE Hardware.CPUStatus
(
 CPUStatusID   SERIAL ,
 SampleTime      TIMESTAMP NOT NULL ,
 CoreID          INT NOT NULL ,
 CoreUtilisation DECIMAL NOT NULL ,
 NodeID       INT NOT NULL ,

PRIMARY KEY (CPUStatusID),
CONSTRAINT FK_385 FOREIGN KEY (NodeID) REFERENCES Node (NodeID)
);






-- ************************************** Hardware.Process

CREATE TABLE Hardware.Process
(
 ProcessID    SERIAL ,
 pID            SMALLINT NOT NULL ,
 WorkingDirectory TEXT NOT NULL ,
 ProcessName    TEXT NOT NULL ,
 Args           TEXT NOT NULL ,
 StartTime      TIMESTAMP NOT NULL ,
 NodeID      INT NOT NULL ,

PRIMARY KEY (ProcessID),
CONSTRAINT UniqueProcessAndStartPerNode UNIQUE (NodeID, pID, StartTime),
CONSTRAINT FK_390 FOREIGN KEY (NodeID) REFERENCES Node (NodeID)
);






-- ************************************** Hardware.Interface

CREATE TABLE Hardware.Interface
(
 InterfaceID SERIAL ,
 Name          TEXT NOT NULL ,
 Type          TEXT NOT NULL ,
 Description   TEXT NOT NULL ,
 IPv4          INET NOT NULL ,
 IPv6          INET NOT NULL ,
 MAC           MACADDR NOT NULL ,
 Speed         BIGINT NOT NULL ,
 NodeID     INT NOT NULL ,

PRIMARY KEY (InterfaceID),
CONSTRAINT UniqueInterfacePerNode UNIQUE (NodeID, Name),
CONSTRAINT FK_381 FOREIGN KEY (NodeID) REFERENCES Node (NodeID)
);






-- ************************************** Hardware.Filesystem

CREATE TABLE Hardware.Filesystem
(
 FilesystemID SERIAL ,
 Name           TEXT NOT NULL ,
 Type           TEXT NOT NULL ,
 Size           BIGINT NOT NULL ,
 NodeID      INT NOT NULL ,

PRIMARY KEY (FilesystemID),
CONSTRAINT UniqueFilesystemPerNode UNIQUE (NodeID, Name),
CONSTRAINT FK_377 FOREIGN KEY (NodeID) REFERENCES Node (NodeID)
);






-- ************************************** ComponentInstance

CREATE TABLE ComponentInstance
(
 ComponentInstanceID SERIAL ,
 Path                TEXT NOT NULL ,
 Name                TEXT NOT NULL ,
 GraphmlID           TEXT NOT NULL,
 ContainerID         INT NOT NULL ,
 ComponentID         INT NOT NULL ,

PRIMARY KEY (ComponentInstanceID),
CONSTRAINT UniquePathPerRun UNIQUE (ComponentID, Path),
CONSTRAINT FK_394 FOREIGN KEY (ContainerID) REFERENCES Container (ContainerID),
CONSTRAINT FK_407 FOREIGN KEY (ComponentID) REFERENCES Component (ComponentID)
);






-- ************************************** Hardware.SystemStatus

CREATE TABLE Hardware.SystemStatus
(
 SystemStatusID   SERIAL ,
 SystemID         INT NOT NULL ,
 SampleTime         TIMESTAMP NOT NULL ,
 CPUUtilisation     DECIMAL NOT NULL ,
 PhysMemUtilisation DECIMAL NOT NULL ,

PRIMARY KEY (SystemStatusID),
CONSTRAINT FK_SystemStatus_SystemID_System_SystemID FOREIGN KEY (SystemID) REFERENCES Hardware.System (SystemID)
);






-- ************************************** Hardware.ProcessStatus

CREATE TABLE Hardware.ProcessStatus
(
 ProcessID        INT NOT NULL ,
 ProcessStatusID  SERIAL ,
 CoreID             INT NOT NULL ,
 CPUutilisation     DECIMAL NOT NULL ,
 CPUtime            INTERVAL NOT NULL ,
 PhysMemUtilisation DECIMAL NOT NULL ,
 PhysMemUsedKB      BIGINT NOT NULL,
 ThreadCount        INTEGER NOT NULL ,
 DiskRead           BIGINT NOT NULL ,
 DiskWritten        BIGINT NOT NULL ,
 DiskTotal          BIGINT NOT NULL ,
 State              TEXT NOT NULL ,
 SampleTime         TIMESTAMP NOT NULL ,

PRIMARY KEY (ProcessStatusID),
CONSTRAINT FK_131 FOREIGN KEY (ProcessID) REFERENCES Hardware.Process (ProcessID)
);






-- ************************************** Hardware.InterfaceStatus

CREATE TABLE Hardware.InterfaceStatus
(
 InterfaceID       INT NOT NULL ,
 InterfaceStatusID SERIAL ,
 PacketsReceived     BIGINT NOT NULL ,
 BytesReceived       BIGINT NOT NULL ,
 PacketsTransmitted  BIGINT NOT NULL ,
 BytesTransmitted    BIGINT NOT NULL ,
 SampleTime          TIMESTAMP NOT NULL ,

PRIMARY KEY (InterfaceStatusID),
CONSTRAINT FK_104 FOREIGN KEY (InterfaceID) REFERENCES Hardware.Interface (InterfaceID)
);






-- ************************************** Hardware.FilesystemStatus

CREATE TABLE Hardware.FilesystemStatus
(
 FilesystemStatusID SERIAL ,
 FilesystemID       INT NOT NULL ,
 SampleTime          TIMESTAMP NOT NULL ,
 Utilisation         DECIMAL NOT NULL ,

PRIMARY KEY (FilesystemStatusID),
CONSTRAINT FK_Filesystem_FileSystem_ID_FileSystem_FilsystemID FOREIGN KEY (FilesystemID) REFERENCES Hardware.Filesystem (FilesystemID)
);







-- ************************************** ComponentLifecycleEvent

CREATE TABLE ComponentLifecycleEvent
(
 ComponentLifecycleEventID SERIAL ,
 ComponentInstanceID       INT NOT NULL ,
 Type                      TEXT NOT NULL ,
 SampleTime                TIMESTAMP NOT NULL ,

PRIMARY KEY (ComponentLifecycleEventID),
CONSTRAINT FK_ComponentLifecycleEvent_ComponentInstanceID_ComponentInstance_ComponentInstanceID FOREIGN KEY (ComponentInstanceID) REFERENCES ComponentInstance (ComponentInstanceID)
);






-- ************************************** UserEvent

CREATE TABLE UserEvent
(
 UserEventID         SERIAL ,
 Message             TEXT NOT NULL ,
 ComponentInstanceID INT NOT NULL ,
 Type                TEXT NOT NULL ,
 SampleTime          TIMESTAMP NOT NULL ,

PRIMARY KEY (UserEventID),
CONSTRAINT FK_UserEvent_ComponentID_Component_ComponentID FOREIGN KEY (ComponentInstanceID) REFERENCES ComponentInstance (ComponentInstanceID)
);






-- ************************************** WorkerInstance

CREATE TABLE WorkerInstance
(
 WorkerInstanceID    SERIAL ,
 Path                TEXT NOT NULL ,
 Name                TEXT NOT NULL ,
 GraphmlID           TEXT NOT NULL,
 ComponentInstanceID INT NOT NULL ,
 WorkerID            INT NOT NULL ,

PRIMARY KEY (WorkerInstanceID),
CONSTRAINT UniqueWorkerNamePerComponent UNIQUE (Name, ComponentInstanceID),
CONSTRAINT FK_Worker_ComponentID_Component_ComponentID FOREIGN KEY (ComponentInstanceID) REFERENCES ComponentInstance (ComponentInstanceID),
CONSTRAINT FK_416 FOREIGN KEY (WorkerID) REFERENCES Worker (WorkerID)
);






-- ************************************** Port

CREATE TABLE Port
(
 PortID              SERIAL ,
 Name                TEXT NOT NULL ,
 Path                TEXT NOT NULL ,
 GraphmlID           TEXT NOT NULL,
 ComponentInstanceID INT NOT NULL ,
 Kind                TEXT NOT NULL ,
 Type                TEXT ,
 Middleware          TEXT ,

PRIMARY KEY (PortID),
CONSTRAINT UniquePortNamePerComponent UNIQUE (ComponentInstanceID, Name),
CONSTRAINT FK_Port_ComponentID_Component_ComponentID FOREIGN KEY (ComponentInstanceID) REFERENCES ComponentInstance (ComponentInstanceID)
);






-- ************************************** PortLifecycleEvent

CREATE TABLE PortLifecycleEvent
(
 PortLifecycleEventID SERIAL ,
 PortID               INT NOT NULL ,
 Type                 TEXT NOT NULL ,
 SampleTime           TIMESTAMP NOT NULL ,

PRIMARY KEY (PortLifecycleEventID),
CONSTRAINT FK_PortLifecycleEvent_PortID_Port_PortID FOREIGN KEY (PortID) REFERENCES Port (PortID)
);






-- ************************************** WorkloadEvent

CREATE TABLE WorkloadEvent
(
 WorkloadEventID  SERIAL ,
 WorkerInstanceID INT NOT NULL ,
 WorkloadID     INT NOT NULL ,
 Function         TEXT NOT NULL ,
 Type             TEXT NOT NULL ,
 Arguments        TEXT NOT NULL ,
 LogLevel         INT NOT NULL ,
 SampleTime       TIMESTAMP NOT NULL ,

PRIMARY KEY (WorkloadEventID),
CONSTRAINT FK_WorkloadEvent_WorkerID_Worker_WorkerID FOREIGN KEY (WorkerInstanceID) REFERENCES WorkerInstance (WorkerInstanceID)
);






-- ************************************** PortEvent

CREATE TABLE PortEvent
(
 PortEventID          SERIAL ,
 PortID               INT NOT NULL ,
 PortEventSequenceNum INT NOT NULL ,
 Type                 TEXT NOT NULL ,
 Message              TEXT ,
 SampleTime           TIMESTAMP NOT NULL ,

PRIMARY KEY (PortEventID),
CONSTRAINT FK_PortEvent_PortID_Port_PortID FOREIGN KEY (PortID) REFERENCES Port (PortID)
);



-- ************************************* PubSubConnection

CREATE TABLE PubSubConnection
(
 PubPortID          INT NOT NULL ,
 SubPortID          INT NOT NULL ,

PRIMARY KEY (PubPortID, SubPortID),
CONSTRAINT FK_PubSubConnection_PubPortID_Port_PortID FOREIGN KEY (PubPortID) REFERENCES Port (PortID),
CONSTRAINT FK_PubSubConnection_SubPortID_Port_PortID FOREIGN KEY (SubPortID) REFERENCES Port (PortID)
);

-- ************************************* ReqRepConnection

CREATE TABLE ReqRepConnection
(
 ReqPortID          INT NOT NULL ,
 RepPortID          INT NOT NULL ,

PRIMARY KEY (ReqPortID, RepPortID),
CONSTRAINT FK_PubSubConnection_ReqPortID_Port_PortID FOREIGN KEY (ReqPortID) REFERENCES Port (PortID),
CONSTRAINT FK_PubSubConnection_RepPortID_Port_PortID FOREIGN KEY (RepPortID) REFERENCES Port (PortID)
);