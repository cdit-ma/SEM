<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" >
 
    <!--
        Purpose:

        This template contains all the data key variables 
		used for graphml representation of the model
    -->
	
	<!-- Assign data keys as used by yEd 3.12.2 -->
    <xsl:variable name="resourcesKey">d0</xsl:variable>
    <xsl:variable name="portGraphicsKey">d1</xsl:variable>
	<xsl:variable name="portGeometryKey">d2</xsl:variable>
	<xsl:variable name="portUserDataKey">d3</xsl:variable>

	<!-- Add model specific data keys for nodes here -->
	
	<xsl:variable name="nodeURLKey">d4</xsl:variable>
	<xsl:variable name="nodeDescriptionKey">d5</xsl:variable>
	<xsl:variable name="nodeGraphicsKey">d6</xsl:variable>

	<!-- Add model specific data keys for graphs here -->

	<xsl:variable name="graphDescriptionKey">d7</xsl:variable>

	<!-- Add model specific data keys for edges here -->

	<xsl:variable name="edgeURLKey">d8</xsl:variable>
	<xsl:variable name="edgeDescriptionKey">d9</xsl:variable>
	<xsl:variable name="edgeGraphicsKey">d10</xsl:variable>
	
	<!-- Add model specific data keys for nodes here -->
	<xsl:variable name="nodeLabelKey">d11</xsl:variable>
	<xsl:variable name="nodeXKey">d12</xsl:variable>
	<xsl:variable name="nodeYKey">d13</xsl:variable>
	<xsl:variable name="nodeKindKey">d14</xsl:variable>
	<xsl:variable name="nodeTypeKey">d15</xsl:variable>
	<xsl:variable name="nodeValueKey">d16</xsl:variable>
	<xsl:variable name="nodeKeyMemberKey">d17</xsl:variable>
	<xsl:variable name="nodeMiddlewareKey">d18</xsl:variable>
	<xsl:variable name="nodeTopicNameKey">d19</xsl:variable>
	<xsl:variable name="nodeAsyncKey">d20</xsl:variable>
	<xsl:variable name="nodeStreamKey">d21</xsl:variable>
	
	<xsl:variable name="nodeFolderKey">d22</xsl:variable>
	<xsl:variable name="nodeFileKey">d23</xsl:variable>
	<xsl:variable name="nodeProjectKey">d24</xsl:variable>
	<xsl:variable name="nodeContainerKey">d25</xsl:variable>
	<xsl:variable name="nodeWorkerKey">d26</xsl:variable>
	<xsl:variable name="nodeOperationKey">d27</xsl:variable>
	<xsl:variable name="nodeActionOnKey">d28</xsl:variable>
	<xsl:variable name="nodeComplexityKey">d29</xsl:variable>
	<xsl:variable name="nodeParametersKey">d30</xsl:variable>
	
	<xsl:variable name="nodeIp_addressKey">d31</xsl:variable>
	<xsl:variable name="nodeOsKey">d32</xsl:variable>
	<xsl:variable name="nodeArchitectureKey">d33</xsl:variable>
	<xsl:variable name="nodeOs_versionKey">d34</xsl:variable>
	<xsl:variable name="nodeShared_directoryKey">d35</xsl:variable>

	<xsl:variable name="nodeProjectUUID">d36</xsl:variable>
	<xsl:variable name="nodeComponentUUID">d37</xsl:variable>
	<xsl:variable name="nodeImplUUID">d38</xsl:variable>
	<xsl:variable name="nodeSvntUUID">d39</xsl:variable>
	<xsl:variable name="nodeComponentInstanceUUID">d40</xsl:variable>
	
	<xsl:variable name="nodeWidthKey">d41</xsl:variable>
	<xsl:variable name="nodeHeightKey">d42</xsl:variable>
	
	<xsl:variable name="nodeSortOrderKey">d43</xsl:variable>
	<xsl:variable name="nodeFrequencyKey">d44</xsl:variable>
	<xsl:variable name="nodeCodeKey">d45</xsl:variable>
	<xsl:variable name="nodeComplexityParametersKey">d46</xsl:variable>
	<xsl:variable name="nodeWorkerIDKey">d47</xsl:variable>
	
	<xsl:variable name="nodeReadOnlyKey">d48</xsl:variable>
	<xsl:variable name="nodeMaxSizeKey">d49</xsl:variable>
	
</xsl:stylesheet>