// Unity Build generated by CMake
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/PanoramaEndpointProvider.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/PanoramaEndpointRules.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/PanoramaErrorMarshaller.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/PanoramaErrors.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/PanoramaRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/AlternateSoftwareMetadata.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ApplicationInstance.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ApplicationInstanceHealthStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ApplicationInstanceStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ConflictException.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ConflictExceptionErrorArgument.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ConnectionType.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/CreateApplicationInstanceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/CreateApplicationInstanceResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/CreateJobForDevicesRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/CreateJobForDevicesResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/CreateNodeFromTemplateJobRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/CreateNodeFromTemplateJobResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/CreatePackageImportJobRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/CreatePackageImportJobResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/CreatePackageRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/CreatePackageResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DeleteDeviceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DeleteDeviceResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DeletePackageRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DeletePackageResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DeregisterPackageVersionRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DeregisterPackageVersionResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DescribeApplicationInstanceDetailsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DescribeApplicationInstanceDetailsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DescribeApplicationInstanceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DescribeApplicationInstanceResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DescribeDeviceJobRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DescribeDeviceJobResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DescribeDeviceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DescribeDeviceResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DescribeNodeFromTemplateJobRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DescribeNodeFromTemplateJobResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DescribeNodeRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DescribeNodeResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DescribePackageImportJobRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DescribePackageImportJobResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DescribePackageRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DescribePackageResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DescribePackageVersionRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DescribePackageVersionResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DesiredState.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/Device.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DeviceAggregatedStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DeviceBrand.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DeviceConnectionStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DeviceJob.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DeviceJobConfig.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DeviceReportedStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DeviceStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/DeviceType.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/EthernetPayload.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/EthernetStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/InternalServerException.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/Job.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/JobResourceTags.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/JobResourceType.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/JobType.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/LatestDeviceJob.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ListApplicationInstanceDependenciesRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ListApplicationInstanceDependenciesResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ListApplicationInstanceNodeInstancesRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ListApplicationInstanceNodeInstancesResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ListApplicationInstancesRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ListApplicationInstancesResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ListDevicesJobsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ListDevicesJobsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ListDevicesRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ListDevicesResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ListDevicesSortBy.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ListNodeFromTemplateJobsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ListNodeFromTemplateJobsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ListNodesRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ListNodesResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ListPackageImportJobsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ListPackageImportJobsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ListPackagesRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ListPackagesResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ListTagsForResourceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ListTagsForResourceResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ManifestOverridesPayload.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ManifestPayload.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/NetworkConnectionStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/NetworkPayload.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/NetworkStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/Node.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/NodeCategory.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/NodeFromTemplateJob.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/NodeFromTemplateJobStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/NodeInputPort.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/NodeInstance.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/NodeInstanceStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/NodeInterface.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/NodeOutputPort.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/NodeSignal.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/NodeSignalValue.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/NtpPayload.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/NtpStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/OTAJobConfig.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/OutPutS3Location.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/PackageImportJob.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/PackageImportJobInputConfig.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/PackageImportJobOutput.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/PackageImportJobOutputConfig.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/PackageImportJobStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/PackageImportJobType.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/PackageListItem.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/PackageObject.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/PackageVersionInputConfig.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/PackageVersionOutputConfig.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/PackageVersionStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/PortType.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ProvisionDeviceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ProvisionDeviceResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/RegisterPackageVersionRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/RegisterPackageVersionResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/RemoveApplicationInstanceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/RemoveApplicationInstanceResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ReportedRuntimeContextState.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ResourceNotFoundException.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/S3Location.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ServiceQuotaExceededException.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/SignalApplicationInstanceNodeInstancesRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/SignalApplicationInstanceNodeInstancesResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/SortOrder.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/StaticIpConnectionInfo.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/StatusFilter.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/StorageLocation.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/TagResourceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/TagResourceResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/TemplateType.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/UntagResourceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/UntagResourceResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/UpdateDeviceMetadataRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/UpdateDeviceMetadataResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/UpdateProgress.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ValidationException.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ValidationExceptionErrorArgument.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ValidationExceptionField.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/model/ValidationExceptionReason.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-panorama/source/PanoramaClient.cpp>
