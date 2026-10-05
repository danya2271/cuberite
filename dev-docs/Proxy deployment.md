# Proxy deployment

For a local Velocity gateway, set `[Server] BindAddress=127.0.0.1` and
`Ports=25566` in `settings.ini`. An empty `BindAddress` preserves the existing
wildcard listener. Numeric IPv4 and IPv6 addresses are accepted; invalid addresses
fail closed instead of falling back to all interfaces.

Use `[Authentication] Authenticate=0`, `AllowBungeeCord=1`,
`OnlyAllowBungeeCord=1`, and a random `ProxySharedSecret`. Configure the same
secret in Velocity with `player-info-forwarding-mode="bungeeguard"` and authenticate
players at the public gateway. Do not expose the Cuberite, WebAdmin, or RCON ports.

Modern client compatibility and native transfers belong to the gateway. This
setting does not add modern protocol support to Cuberite.
