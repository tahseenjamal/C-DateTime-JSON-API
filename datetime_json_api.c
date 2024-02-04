#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <microhttpd.h>


#define BUFFER_SIZE 512

// Thread-specific data key for time zone
static pthread_key_t tz_key;

// Thread-specific destructor function
static void free_tz(void *tz) {
    free(tz);
}

// Function to set time zone for the current thread
void setTimeZone(const char *timezone) {
    // Allocate memory for the time zone string
    char *tz = strdup(timezone);

    // Set the time zone in thread-specific data
    pthread_setspecific(tz_key, tz);

    // Update the time zone information
    setenv("TZ", tz, 1);
    tzset();
}

int answer_to_connection(void *cls, struct MHD_Connection *connection,
                         const char *url, const char *method,
                         const char *version, const char *upload_data,
                         size_t *upload_data_size, void **con_cls)
{
    char page[BUFFER_SIZE] = "";
    char time_str[512];
    time_t t;
    struct tm *timeinfo;
    struct MHD_Response *response;
    const char *timezone_param;
    int ret;

    if (strcmp(method, "GET"))
        return MHD_NO; /* unexpected method */

    // Get the time zone parameter from the query string
    timezone_param = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "timezone");

    // Use the specified time zone or default to local time
    t = time(NULL);
    if (timezone_param != NULL) {
        setTimeZone(timezone_param);
    }

    timeinfo = localtime(&t);

    // Use %z for timezone offset in the format string
    strftime(time_str, sizeof(time_str), "%Y-%m-%dT%H:%M:%S%z", timeinfo);

    snprintf(page, sizeof(page), "{\"time\":\"%s\"}", time_str);

    response = MHD_create_response_from_buffer(strlen(page), (void *)page, MHD_RESPMEM_MUST_COPY);
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    return ret;
}

int main()
{
    // Initialize thread-specific data key
    pthread_key_create(&tz_key, free_tz);

    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, 8080, NULL, NULL,
                              (MHD_AccessHandlerCallback)&answer_to_connection, NULL, MHD_OPTION_END);
    if (NULL == daemon)
        return 1;

    (void)getchar(); /* wait for a key press before exiting */

    MHD_stop_daemon(daemon);

    // Clean up thread-specific data key
    pthread_key_delete(tz_key);

    return 0;
}

