#include <stdio.h>
#include <xmlrpc.h>
#include <xmlrpc_client.h>
    
#define NAME       "XML-RPC Device Data Gathering C Client"
#define VERSION    "1.0"
#define SERVER_URL "http://192.168.2.151/device"
    
void die_if_fault_occurred (xmlrpc_env *env)
{
    /* Check our error-handling environment for an XML-RPC fault. */
    if (env->fault_occurred) {
        fprintf(stderr, "XML-RPC Fault: %s (%d)\n",
                env->fault_string, env->fault_code);
        exit(1);
    }
}
    
#define BASIC_STATS	0

int main (int argc, char** argv)
{
    xmlrpc_env env;
    xmlrpc_value *result;
    xmlrpc_int32 anint, abool, status;
    double d;
    char *string, *lastCommand, *currentState;
    
    /* Start up our XML-RPC client library. */
    xmlrpc_client_init(XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION);
    xmlrpc_env_init(&env);

    /* Call our XML-RPC server. */

    result = xmlrpc_client_call(&env, SERVER_URL,
                                 "getDeviceStats", "(ssi)",
                                 "username", "password",
                                 BASIC_STATS );

    die_if_fault_occurred(&env);
    
    /* Parse our result value. */
    xmlrpc_parse_value(&env, result, "{s:i,s:s,s:s,*}",
                        "status", &status, "lastCommand", &lastCommand, 
                        "currentState", &currentState);

    die_if_fault_occurred(&env);

    printf("Status : %d\n", status);
    printf("Last Command was : %s\n", lastCommand);
    printf("Current State is : %s\n\n", currentState);

    /* Dispose of our result value. */
    xmlrpc_DECREF(result);

    /* Shutdown our XML-RPC client library. */
    xmlrpc_env_clean(&env);
    xmlrpc_client_cleanup();

    return 0;
}

