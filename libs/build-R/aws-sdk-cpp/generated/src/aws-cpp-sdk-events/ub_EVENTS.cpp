// Unity Build generated by CMake
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/CloudWatchEventsEndpointProvider.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/CloudWatchEventsEndpointRules.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/CloudWatchEventsErrorMarshaller.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/CloudWatchEventsErrors.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/CloudWatchEventsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ActivateEventSourceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ApiDestination.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ApiDestinationHttpMethod.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ApiDestinationState.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/Archive.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ArchiveState.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/AssignPublicIp.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/AwsVpcConfiguration.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/BatchArrayProperties.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/BatchParameters.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/BatchRetryStrategy.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/CancelReplayRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/CancelReplayResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/CapacityProviderStrategyItem.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/Condition.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/Connection.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ConnectionApiKeyAuthResponseParameters.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ConnectionAuthResponseParameters.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ConnectionAuthorizationType.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ConnectionBasicAuthResponseParameters.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ConnectionBodyParameter.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ConnectionHeaderParameter.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ConnectionHttpParameters.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ConnectionOAuthClientResponseParameters.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ConnectionOAuthHttpMethod.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ConnectionOAuthResponseParameters.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ConnectionQueryStringParameter.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ConnectionState.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/CreateApiDestinationRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/CreateApiDestinationResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/CreateArchiveRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/CreateArchiveResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/CreateConnectionApiKeyAuthRequestParameters.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/CreateConnectionAuthRequestParameters.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/CreateConnectionBasicAuthRequestParameters.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/CreateConnectionOAuthClientRequestParameters.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/CreateConnectionOAuthRequestParameters.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/CreateConnectionRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/CreateConnectionResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/CreateEventBusRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/CreateEventBusResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/CreatePartnerEventSourceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/CreatePartnerEventSourceResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DeactivateEventSourceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DeadLetterConfig.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DeauthorizeConnectionRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DeauthorizeConnectionResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DeleteApiDestinationRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DeleteApiDestinationResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DeleteArchiveRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DeleteArchiveResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DeleteConnectionRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DeleteConnectionResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DeleteEventBusRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DeletePartnerEventSourceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DeleteRuleRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DescribeApiDestinationRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DescribeApiDestinationResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DescribeArchiveRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DescribeArchiveResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DescribeConnectionRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DescribeConnectionResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DescribeEventBusRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DescribeEventBusResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DescribeEventSourceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DescribeEventSourceResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DescribePartnerEventSourceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DescribePartnerEventSourceResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DescribeReplayRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DescribeReplayResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DescribeRuleRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DescribeRuleResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/DisableRuleRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/EcsParameters.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/EnableRuleRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/EventBus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/EventSource.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/EventSourceState.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/HttpParameters.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/InputTransformer.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/KinesisParameters.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/LaunchType.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ListApiDestinationsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ListApiDestinationsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ListArchivesRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ListArchivesResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ListConnectionsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ListConnectionsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ListEventBusesRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ListEventBusesResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ListEventSourcesRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ListEventSourcesResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ListPartnerEventSourceAccountsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ListPartnerEventSourceAccountsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ListPartnerEventSourcesRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ListPartnerEventSourcesResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ListReplaysRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ListReplaysResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ListRuleNamesByTargetRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ListRuleNamesByTargetResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ListRulesRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ListRulesResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ListTagsForResourceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ListTagsForResourceResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ListTargetsByRuleRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ListTargetsByRuleResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/NetworkConfiguration.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/PartnerEventSource.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/PartnerEventSourceAccount.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/PlacementConstraint.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/PlacementConstraintType.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/PlacementStrategy.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/PlacementStrategyType.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/PropagateTags.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/PutEventsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/PutEventsRequestEntry.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/PutEventsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/PutEventsResultEntry.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/PutPartnerEventsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/PutPartnerEventsRequestEntry.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/PutPartnerEventsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/PutPartnerEventsResultEntry.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/PutPermissionRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/PutRuleRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/PutRuleResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/PutTargetsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/PutTargetsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/PutTargetsResultEntry.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/RedshiftDataParameters.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/RemovePermissionRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/RemoveTargetsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/RemoveTargetsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/RemoveTargetsResultEntry.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/Replay.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ReplayDestination.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/ReplayState.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/RetryPolicy.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/Rule.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/RuleState.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/RunCommandParameters.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/RunCommandTarget.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/SageMakerPipelineParameter.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/SageMakerPipelineParameters.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/SqsParameters.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/StartReplayRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/StartReplayResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/Tag.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/TagResourceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/TagResourceResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/Target.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/TestEventPatternRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/TestEventPatternResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/UntagResourceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/UntagResourceResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/UpdateApiDestinationRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/UpdateApiDestinationResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/UpdateArchiveRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/UpdateArchiveResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/UpdateConnectionApiKeyAuthRequestParameters.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/UpdateConnectionAuthRequestParameters.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/UpdateConnectionBasicAuthRequestParameters.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/UpdateConnectionOAuthClientRequestParameters.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/UpdateConnectionOAuthRequestParameters.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/UpdateConnectionRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/model/UpdateConnectionResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-events/source/CloudWatchEventsClient.cpp>
