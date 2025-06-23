# URLShortener
A fully asynchronous server and TLS-enabled client microservice in C++.

The input is a json payload containing:
| param    | type   | required | description                        |
|----------|--------|----------|------------------------------------|
| url      | string | Y        | The URL to shorten                 |
| provider | string | N        | The provider to use for shortening |

## Response

On **success** (`result == 0`)
| param     | type   | required | description                                 |
|-----------|--------|----------|---------------------------------------------|
| result    | int    | Y        | The original URL                            |
| reason    | string | Y        | The original URL                            |
| data      | object | Y        | Payload object containing the shortened URL |

** `data` object**:
| url       | string | Y        | The original URL                     |
| shortened | string | Y        | The shortened link                   |

```json
{
    "result": 0,
    "reason": "",
    "data":
    {
        "url": "https://example.com",
        "shortened": "https://bit.ly/8h1bka"
    }
}
```

On **error** (`result == 1`)
| param     | type   | required | description                                 |
|-----------|--------|----------|---------------------------------------------|
| result    | int    | Y        | The original URL                            |
| reason    | string | Y        | The original URL                            |
| data      | object | N        | Payload object containing the shortened URL |

```json
{
    "result": 1,
    "reason": "JSON must contain key url",
}
```

# Default provider
```
curl -X 'POST' "Content-Type: application/json" localhost:8080/shortly -d '{"url": "https://www.google.com"}'
```
# Explicit provider
```
curl -vvv -X 'POST' "Content-Type: application/json" localhost:8080/shortly -d '{"url": "https://www.google.com", "provider": "tinyurl"}'
```

# Configuration
You need two API keys:
[bit.ly](https://dev.bitly.com/)           -> set `API_KEY_BITLY`
[tinyurl.com](https://tinyurl.com/app/dev) -> set `API_KEY_TINYURL`

```
export API_KEY_BITLY="your-bitly-key"
export API_KEY_TINYURL="your-tinyurl-key"
```

# Build & Run
make
make deploy

# Technologies & Design
* C++20 and coroutines
* Boost.Asio & Boost.Beast
* Coroutine-driven I/O
* Hexagonal Architecture
* Patterns:
    * Strategy
    * Factory
    * Template
    * Adapter
    * DTO