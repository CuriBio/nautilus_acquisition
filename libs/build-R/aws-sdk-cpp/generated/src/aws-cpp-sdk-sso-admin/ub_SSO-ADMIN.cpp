// Unity Build generated by CMake
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/SSOAdminEndpointProvider.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/SSOAdminEndpointRules.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/SSOAdminErrorMarshaller.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/SSOAdminErrors.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/SSOAdminRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/AccessControlAttribute.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/AccessControlAttributeValue.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/AccountAssignment.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/AccountAssignmentForPrincipal.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/AccountAssignmentOperationStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/AccountAssignmentOperationStatusMetadata.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/Application.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ApplicationAssignment.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ApplicationAssignmentForPrincipal.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ApplicationProvider.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ApplicationStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ApplicationVisibility.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/AttachCustomerManagedPolicyReferenceToPermissionSetRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/AttachCustomerManagedPolicyReferenceToPermissionSetResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/AttachManagedPolicyToPermissionSetRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/AttachManagedPolicyToPermissionSetResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/AttachedManagedPolicy.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/AuthenticationMethod.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/AuthenticationMethodItem.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/AuthenticationMethodType.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/AuthorizationCodeGrant.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/AuthorizedTokenIssuer.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/CreateAccountAssignmentRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/CreateAccountAssignmentResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/CreateApplicationAssignmentRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/CreateApplicationAssignmentResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/CreateApplicationRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/CreateApplicationResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/CreateInstanceAccessControlAttributeConfigurationRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/CreateInstanceAccessControlAttributeConfigurationResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/CreateInstanceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/CreateInstanceResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/CreatePermissionSetRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/CreatePermissionSetResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/CreateTrustedTokenIssuerRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/CreateTrustedTokenIssuerResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/CustomerManagedPolicyReference.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DeleteAccountAssignmentRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DeleteAccountAssignmentResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DeleteApplicationAccessScopeRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DeleteApplicationAssignmentRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DeleteApplicationAssignmentResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DeleteApplicationAuthenticationMethodRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DeleteApplicationGrantRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DeleteApplicationRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DeleteApplicationResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DeleteInlinePolicyFromPermissionSetRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DeleteInlinePolicyFromPermissionSetResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DeleteInstanceAccessControlAttributeConfigurationRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DeleteInstanceAccessControlAttributeConfigurationResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DeleteInstanceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DeleteInstanceResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DeletePermissionSetRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DeletePermissionSetResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DeletePermissionsBoundaryFromPermissionSetRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DeletePermissionsBoundaryFromPermissionSetResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DeleteTrustedTokenIssuerRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DeleteTrustedTokenIssuerResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DescribeAccountAssignmentCreationStatusRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DescribeAccountAssignmentCreationStatusResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DescribeAccountAssignmentDeletionStatusRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DescribeAccountAssignmentDeletionStatusResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DescribeApplicationAssignmentRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DescribeApplicationAssignmentResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DescribeApplicationProviderRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DescribeApplicationProviderResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DescribeApplicationRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DescribeApplicationResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DescribeInstanceAccessControlAttributeConfigurationRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DescribeInstanceAccessControlAttributeConfigurationResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DescribeInstanceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DescribeInstanceResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DescribePermissionSetProvisioningStatusRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DescribePermissionSetProvisioningStatusResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DescribePermissionSetRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DescribePermissionSetResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DescribeTrustedTokenIssuerRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DescribeTrustedTokenIssuerResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DetachCustomerManagedPolicyReferenceFromPermissionSetRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DetachCustomerManagedPolicyReferenceFromPermissionSetResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DetachManagedPolicyFromPermissionSetRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DetachManagedPolicyFromPermissionSetResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/DisplayData.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/FederationProtocol.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/GetApplicationAccessScopeRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/GetApplicationAccessScopeResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/GetApplicationAssignmentConfigurationRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/GetApplicationAssignmentConfigurationResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/GetApplicationAuthenticationMethodRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/GetApplicationAuthenticationMethodResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/GetApplicationGrantRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/GetApplicationGrantResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/GetInlinePolicyForPermissionSetRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/GetInlinePolicyForPermissionSetResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/GetPermissionsBoundaryForPermissionSetRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/GetPermissionsBoundaryForPermissionSetResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/Grant.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/GrantItem.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/GrantType.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/IamAuthenticationMethod.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/InstanceAccessControlAttributeConfiguration.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/InstanceAccessControlAttributeConfigurationStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/InstanceMetadata.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/InstanceStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/JwksRetrievalOption.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/JwtBearerGrant.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListAccountAssignmentCreationStatusRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListAccountAssignmentCreationStatusResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListAccountAssignmentDeletionStatusRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListAccountAssignmentDeletionStatusResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListAccountAssignmentsFilter.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListAccountAssignmentsForPrincipalRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListAccountAssignmentsForPrincipalResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListAccountAssignmentsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListAccountAssignmentsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListAccountsForProvisionedPermissionSetRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListAccountsForProvisionedPermissionSetResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListApplicationAccessScopesRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListApplicationAccessScopesResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListApplicationAssignmentsFilter.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListApplicationAssignmentsForPrincipalRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListApplicationAssignmentsForPrincipalResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListApplicationAssignmentsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListApplicationAssignmentsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListApplicationAuthenticationMethodsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListApplicationAuthenticationMethodsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListApplicationGrantsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListApplicationGrantsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListApplicationProvidersRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListApplicationProvidersResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListApplicationsFilter.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListApplicationsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListApplicationsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListCustomerManagedPolicyReferencesInPermissionSetRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListCustomerManagedPolicyReferencesInPermissionSetResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListInstancesRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListInstancesResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListManagedPoliciesInPermissionSetRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListManagedPoliciesInPermissionSetResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListPermissionSetProvisioningStatusRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListPermissionSetProvisioningStatusResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListPermissionSetsProvisionedToAccountRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListPermissionSetsProvisionedToAccountResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListPermissionSetsRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListPermissionSetsResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListTagsForResourceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListTagsForResourceResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListTrustedTokenIssuersRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ListTrustedTokenIssuersResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/OidcJwtConfiguration.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/OidcJwtUpdateConfiguration.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/OperationStatusFilter.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/PermissionSet.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/PermissionSetProvisioningStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/PermissionSetProvisioningStatusMetadata.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/PermissionsBoundary.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/PortalOptions.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/PrincipalType.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ProvisionPermissionSetRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ProvisionPermissionSetResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ProvisionTargetType.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ProvisioningStatus.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/PutApplicationAccessScopeRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/PutApplicationAssignmentConfigurationRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/PutApplicationAssignmentConfigurationResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/PutApplicationAuthenticationMethodRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/PutApplicationGrantRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/PutInlinePolicyToPermissionSetRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/PutInlinePolicyToPermissionSetResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/PutPermissionsBoundaryToPermissionSetRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/PutPermissionsBoundaryToPermissionSetResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/RefreshTokenGrant.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ResourceServerConfig.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ResourceServerScopeDetails.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/ScopeDetails.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/SignInOptions.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/SignInOrigin.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/StatusValues.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/Tag.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/TagResourceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/TagResourceResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/TargetType.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/TokenExchangeGrant.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/TrustedTokenIssuerConfiguration.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/TrustedTokenIssuerMetadata.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/TrustedTokenIssuerType.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/TrustedTokenIssuerUpdateConfiguration.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/UntagResourceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/UntagResourceResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/UpdateApplicationPortalOptions.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/UpdateApplicationRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/UpdateApplicationResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/UpdateInstanceAccessControlAttributeConfigurationRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/UpdateInstanceAccessControlAttributeConfigurationResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/UpdateInstanceRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/UpdateInstanceResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/UpdatePermissionSetRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/UpdatePermissionSetResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/UpdateTrustedTokenIssuerRequest.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/model/UpdateTrustedTokenIssuerResult.cpp>
#include <D:/Users/nautilus/Desktop/nautilus_acquisition/libs/aws-sdk-cpp/generated/src/aws-cpp-sdk-sso-admin/source/SSOAdminClient.cpp>
