### Author: Tahseen Jamal

Requirement
```
gcc
microhttpd
```
You need to install them on your respective system


How to compile

```gcc datetime_json_api.c -o datetime_json_api -lmicrohttpd ```

How to run

```./datetime_json_api```

Go to your browser of choice and use below as example

http://localhost:8080/?timezone=Australia/Perth
http://localhost:8080/?timezone=Africa/Accra

In case no timezone is provided it defaults to 

For more timezones, you can check this link and use TZ identifier

https://en.wikipedia.org/wiki/List_of_tz_database_time_zones


