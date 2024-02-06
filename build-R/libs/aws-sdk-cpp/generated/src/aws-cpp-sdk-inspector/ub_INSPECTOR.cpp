// Unity Build generated by CMake
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/InspectorEndpointProvider.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/InspectorEndpointRules.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/InspectorErrorMarshaller.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/InspectorErrors.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/InspectorRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/AccessDeniedErrorCode.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/AccessDeniedException.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/AddAttributesToFindingsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/AddAttributesToFindingsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/AgentAlreadyRunningAssessment.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/AgentFilter.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/AgentHealth.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/AgentHealthCode.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/AgentPreview.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/AgentsAlreadyRunningAssessmentException.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/AssessmentRun.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/AssessmentRunAgent.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/AssessmentRunFilter.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/AssessmentRunInProgressException.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/AssessmentRunNotification.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/AssessmentRunNotificationSnsStatusCode.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/AssessmentRunState.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/AssessmentRunStateChange.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/AssessmentTarget.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/AssessmentTargetFilter.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/AssessmentTemplate.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/AssessmentTemplateFilter.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/AssetAttributes.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/AssetType.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/Attribute.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/CreateAssessmentTargetRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/CreateAssessmentTargetResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/CreateAssessmentTemplateRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/CreateAssessmentTemplateResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/CreateExclusionsPreviewRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/CreateExclusionsPreviewResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/CreateResourceGroupRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/CreateResourceGroupResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/DeleteAssessmentRunRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/DeleteAssessmentTargetRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/DeleteAssessmentTemplateRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/DescribeAssessmentRunsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/DescribeAssessmentRunsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/DescribeAssessmentTargetsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/DescribeAssessmentTargetsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/DescribeAssessmentTemplatesRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/DescribeAssessmentTemplatesResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/DescribeCrossAccountAccessRoleRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/DescribeCrossAccountAccessRoleResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/DescribeExclusionsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/DescribeExclusionsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/DescribeFindingsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/DescribeFindingsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/DescribeResourceGroupsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/DescribeResourceGroupsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/DescribeRulesPackagesRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/DescribeRulesPackagesResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/DurationRange.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/EventSubscription.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/Exclusion.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/ExclusionPreview.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/FailedItemDetails.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/FailedItemErrorCode.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/Finding.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/FindingFilter.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/GetAssessmentReportRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/GetAssessmentReportResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/GetExclusionsPreviewRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/GetExclusionsPreviewResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/GetTelemetryMetadataRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/GetTelemetryMetadataResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/InspectorEvent.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/InspectorServiceAttributes.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/InternalException.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/InvalidCrossAccountRoleErrorCode.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/InvalidCrossAccountRoleException.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/InvalidInputErrorCode.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/InvalidInputException.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/LimitExceededErrorCode.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/LimitExceededException.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/ListAssessmentRunAgentsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/ListAssessmentRunAgentsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/ListAssessmentRunsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/ListAssessmentRunsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/ListAssessmentTargetsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/ListAssessmentTargetsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/ListAssessmentTemplatesRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/ListAssessmentTemplatesResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/ListEventSubscriptionsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/ListEventSubscriptionsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/ListExclusionsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/ListExclusionsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/ListFindingsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/ListFindingsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/ListRulesPackagesRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/ListRulesPackagesResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/ListTagsForResourceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/ListTagsForResourceResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/Locale.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/NetworkInterface.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/NoSuchEntityErrorCode.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/NoSuchEntityException.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/PreviewAgentsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/PreviewAgentsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/PreviewStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/PrivateIp.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/RegisterCrossAccountAccessRoleRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/RemoveAttributesFromFindingsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/RemoveAttributesFromFindingsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/ReportFileFormat.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/ReportStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/ReportType.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/ResourceGroup.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/ResourceGroupTag.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/RulesPackage.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/Scope.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/ScopeType.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/SecurityGroup.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/ServiceTemporarilyUnavailableException.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/SetTagsForResourceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/Severity.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/StartAssessmentRunRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/StartAssessmentRunResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/StopAction.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/StopAssessmentRunRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/SubscribeToEventRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/Subscription.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/Tag.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/TelemetryMetadata.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/TimestampRange.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/UnsubscribeFromEventRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/UnsupportedFeatureException.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/model/UpdateAssessmentTargetRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-inspector/source/InspectorClient.cpp>
