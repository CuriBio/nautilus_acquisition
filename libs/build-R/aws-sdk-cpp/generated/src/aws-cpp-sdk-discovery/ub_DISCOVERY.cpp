// Unity Build generated by CMake
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/ApplicationDiscoveryServiceEndpointProvider.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/ApplicationDiscoveryServiceEndpointRules.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/ApplicationDiscoveryServiceErrorMarshaller.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/ApplicationDiscoveryServiceErrors.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/ApplicationDiscoveryServiceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/AgentConfigurationStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/AgentInfo.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/AgentNetworkInfo.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/AgentStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/AssociateConfigurationItemsToApplicationRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/AssociateConfigurationItemsToApplicationResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/BatchDeleteAgentError.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/BatchDeleteAgentsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/BatchDeleteAgentsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/BatchDeleteConfigurationTask.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/BatchDeleteConfigurationTaskStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/BatchDeleteImportDataError.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/BatchDeleteImportDataErrorCode.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/BatchDeleteImportDataRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/BatchDeleteImportDataResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/ConfigurationItemType.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/ConfigurationTag.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/ContinuousExportDescription.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/ContinuousExportStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/CreateApplicationRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/CreateApplicationResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/CreateTagsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/CreateTagsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/CustomerAgentInfo.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/CustomerAgentlessCollectorInfo.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/CustomerConnectorInfo.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/CustomerMeCollectorInfo.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/DataSource.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/DeleteAgent.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/DeleteAgentErrorCode.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/DeleteApplicationsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/DeleteApplicationsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/DeleteTagsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/DeleteTagsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/DeletionConfigurationItemType.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/DeletionWarning.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/DescribeAgentsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/DescribeAgentsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/DescribeBatchDeleteConfigurationTaskRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/DescribeBatchDeleteConfigurationTaskResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/DescribeConfigurationsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/DescribeConfigurationsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/DescribeContinuousExportsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/DescribeContinuousExportsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/DescribeExportTasksRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/DescribeExportTasksResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/DescribeImportTasksRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/DescribeImportTasksResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/DescribeTagsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/DescribeTagsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/DisassociateConfigurationItemsFromApplicationRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/DisassociateConfigurationItemsFromApplicationResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/Ec2RecommendationsExportPreferences.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/ExportDataFormat.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/ExportFilter.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/ExportInfo.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/ExportPreferences.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/ExportStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/FailedConfiguration.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/Filter.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/GetDiscoverySummaryRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/GetDiscoverySummaryResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/ImportStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/ImportTask.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/ImportTaskFilter.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/ImportTaskFilterName.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/ListConfigurationsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/ListConfigurationsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/ListServerNeighborsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/ListServerNeighborsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/NeighborConnectionDetail.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/OfferingClass.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/OrderByElement.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/OrderString.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/PurchasingOption.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/ReservedInstanceOptions.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/StartBatchDeleteConfigurationTaskRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/StartBatchDeleteConfigurationTaskResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/StartContinuousExportRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/StartContinuousExportResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/StartDataCollectionByAgentIdsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/StartDataCollectionByAgentIdsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/StartExportTaskRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/StartExportTaskResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/StartImportTaskRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/StartImportTaskResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/StopContinuousExportRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/StopContinuousExportResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/StopDataCollectionByAgentIdsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/StopDataCollectionByAgentIdsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/Tag.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/TagFilter.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/Tenancy.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/TermLength.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/UpdateApplicationRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/UpdateApplicationResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/model/UsageMetricBasis.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-discovery/source/ApplicationDiscoveryServiceClient.cpp>
