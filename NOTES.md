# Notes

**Design:** We use a 162-byte custom wire format (2-byte sequence number) to maximize available bandwidth. We use this headroom to immediately send a duplicate copy of 95% of frames as soon as they arrive at the sender, achieving a 1.97x overhead. Because the FEC is transmitted immediately rather than piggybacked on future frames, our receiver does not need a timer-based jitter buffer; it instantly plays out whatever arrives, allowing us to achieve a near-zero FEC delay penalty.

**Grade Delay:** Please grade at `--delay_ms 85`. This is safely above the max network delays of moderate profiles like B.json, while still providing an aggressive score.

**What Breaks It:** Burst losses. Because our duplicates are sent 0ms apart (back-to-back), a network drop event that lasts longer than a few milliseconds has a high chance of swallowing both the original packet and its duplicate simultaneously, causing an unrecoverable deadline miss.
