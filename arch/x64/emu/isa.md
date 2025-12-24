# Section 0
## Register operations
| Operation code (hex) | Operation name | Parameters        | Description                                         |
| -------------------- | -------------- | ----------------- | --------------------------------------------------- |
| `0`                  | `setb`         | `r8_des v8`       | v8 を r8_des にセットする (r8_des = v8)             |
| `1`                  | `setw`         | `r16_des v16`     | v16 を r16_des にセットする (r16_des = v16)         |
| `2`                  | `setl`         | `r32_des v32`     | v32 を r32_des にセットする (r32_des = v32)         |
| `3`                  | `setq`         | `r64_des v64`     | v64 を r64_des にセットする (r64_des = v64)         |
| `4`                  | `cpb`          | `r8_des r8_src`   | r8_src を r8_des にコピーする (r8_des = r8_src)     |
| `5`                  | `cpw`          | `r16_des r16_src` | r16_src を r16_des にコピーする (r16_des = r16_src) |
| `6`                  | `cpl`          | `r32_des r32_src` | r32_src を r32_des にコピーする (r32_des = r32_src) |
| `7`                  | `cpq`          | `r64_des r64_src` | r64_src を r64_des にコピーする (r64_des = r64_src) |

## Memory Operations
| Operation code (hex) | Operation name | Parameters | Description             |
| -------------------- | -------------- | ---------- | ----------------------- |
| `8`                  | `storeb`       | `a64 v8`   | v8 を a64 に書き込む    |
| `9`                  | `storew`       | `a64 v16`  | v16 を a64 に書き込む   |
| `10`                 | `storel`       | `a64 v32`  | v32 を a64 に書き込む   |
| `11`                 | `storeq`       | `a64 v64`  | v64 を a64 に書き込む   |
| `12`                 | `loadb`        | `r8 a64`   | a64 から r8 に読み込む  |
| `13`                 | `loadw`        | `r16 a64`  | a64 から r16 に読み込む |
| `14`                 | `loadl`        | `r32 a64`  | a64 から r32 に読み込む |
| `15`                 | `loadq`        | `r64 a64`  | a64 から r64 に読み込む |
