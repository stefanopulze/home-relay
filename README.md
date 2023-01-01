# Home Relay
Rest server implentation for remote relays shield.

My currenct configuration is:
- wemos d1 mini pro
- 4 relays shield

## Config
`include/config.h.sample` need to be compied into `include/config.h` and filled whit values that suite your environment like wifi and ota password.

## Rest API
**Status**
```http
GET /status

[
    {
        relay: 0,
        ison: true
    },
    ..
    {
        relay: 3,
        ison: false
    }
]
```

**Single relay status**
```http
GET /relay/0

{
    "ison": true
}
```


**Change relay status**
```http 
POST /relay/0

{
    "turn": "on",
    "timer": 1000
}
```

*turn* can be: "on", "off" or "toggle"
*timer* is a optional number for a one-shot flip-back timer in seconds