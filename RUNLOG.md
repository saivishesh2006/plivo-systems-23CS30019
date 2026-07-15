# Run Log

### Experiment 1: Baseline
- **Profile:** A.json
- **Delay:** 40ms
- **Miss %:** 2.00%
- **Overhead:** 1.02x
- **Changes & Why:** Ran the provided baseline. It sends everything exactly once and directly forwards what arrives. Fails because the baseline doesn't use any FEC, meaning the random 2% loss directly translates to a 2% deadline miss rate (failing the < 1% cap).

### Experiment 2: N-1 Piggybacked FEC
- **Profile:** A.json
- **Delay:** 60ms
- **Miss %:** 0.04%
- **Overhead:** 1.98x
- **Changes & Why:** Appended the payload of frame `i-1` onto frame `i`. Since we wait for frame `i` to recover `i-1`, this requires an extra 20ms of playout delay. It succeeded in making the miss rate incredibly low while staying under the 2.0x overhead cap, but the delay was unnecessarily high.

### Experiment 3: Immediate 95% Duplication
- **Profile:** A.json
- **Delay:** 41ms
- **Miss %:** 0.27%
- **Overhead:** 1.97x
- **Changes & Why:** Switched to a custom 162-byte wire format (2-byte seq) and immediately duplicated 95% of frames. This eliminated the 20ms FEC-wait penalty, allowing us to bring `delay_ms` all the way down to 41ms (the theoretical minimum for A.json's 40ms network delay constraint).

### Experiment 4: Stress testing on B.json
- **Profile:** B.json
- **Delay:** 81ms
- **Miss %:** 0.80%
- **Overhead:** 1.97x
- **Changes & Why:** Tested our immediate duplication strategy against a harsher profile (5% loss, 80ms max delay). By setting `delay_ms` to 81ms (to account for the higher natural network delay), our immediate duplication still kept the miss rate under 1%. 

