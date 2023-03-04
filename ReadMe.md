# Hatofi DB

## What ?

Hatofi, stands for "Hashmap to Filesystem" is a key-value database running on Linux. Aims to reduce IO latency from traditional SQL database with less financial cost compared to Redis (stored in filesystem instead of RAM)

## Usage

- Database creation via config file via `hatofi --data_directory <data_dir> gen --config <config_file.txt>`
- Import data using `hatofi --data_directory <data_dir> load --input <input.txt>`
- Query data (partial or exact match) using `hatofi --data_directory <data_dir> query [--exact|--partial] <your_search>`
- Evaluate optimum quartile distribution of data based on their length on a given sample (100 values minimum is recommended) with `hatofi optimize --input <sample.txt>`

## Input Format

Input file format:
```txt
<entity_dataclass>:<entity_key>:<entity_raw_length><dataclass>:<base64_encoded_data>:<data_md5>
```
Assuming current date is `2022-01-01`

Assuming file UUIDv4 is `693ad1be-c353-4562-b12a-930f2ed43b79`

Example input.txt:
```txt
693ad1be-c353-4562-b12a-930f2ed43b79
email:13:20bacbe5082d09eb3ac96a4565c1dc33:email:ZGF0YTFAbmV0LmNvbQ==:20bacbe5082d09eb3ac96a4565c1dc33
email:13:20bacbe5082d09eb3ac96a4565c1dc33:password:cGFzc3dvcmQ=:5f4dcc3b5aa765d61d8327deb882cf99
email:15:ae68135e4f74eed19a79fd982c7c4f98:email:ZGF0YTJAZW1haWwuY29t:ae68135e4f74eed19a79fd982c7c4f98
email:15:1e77fd2c7a59f06a6c8dc8ace3ebf221:email:ZGF0YTNAZW1haWwuY29t:1e77fd2c7a59f06a6c8dc8ace3ebf221
email:16:26af7d285fa312aa2f8d3857d0f00af4:email:ZGF0YTRAZG9tYWluLmNvbQ==:26af7d285fa312aa2f8d3857d0f00af4
email:16::26af7d285fa312aa2f8d3857d0f00af4:password:YW5hd2Vzb21lcGFzc3dvcmQ=:90282e03043af181c985c9891c52c00f
```

```bash
$ md5sum input.txt 
8998c19bb14b9af66ccdc79bed5818c4  input.txt
$ sha256sum input.txt
99226b116e370a25130cce7e55fe3f813a0f3168c30e584de422e9f43b76fc1a input.txt
```

## Output Format

Desired filesystem output architecture:
```txt
> db/
    .dbmeta Database configuration file
    > data/
        > dataclasses/
            > email/
                > _lower_or_equal_8/
                > _lower_or_equal_10/
                > _lower_or_equal_12/
                > _more_than_12/
                    > 1e77fd2c7a59f06a6c8dc8ace3ebf221 -> ../1e/77/1e77fd2c7a59f06a6c8dc8ace3ebf221
                    > 20bacbe5082d09eb3ac96a4565c1dc33 -> ../20/ba/20bacbe5082d09eb3ac96a4565c1dc33
                    > 26af7d285fa312aa2f8d3857d0f00af4 -> ../26/af/26af7d285fa312aa2f8d3857d0f00af4
                    > ae68135e4f74eed19a79fd982c7c4f98 -> ../ae/68/ae68135e4f74eed19a79fd982c7c4f98
                > xx/xx/
                    > lower_or_equal_8/
                    > lower_or_equal_10/
                    > lower_or_equal_12/
                    > more_than_12/
                > 1e/77/
                    > 1e77fd2c7a59f06a6c8dc8ace3ebf221/
                        > 1e77fd2c7a59f06a6c8dc8ace3ebf221.md5 << b64:ZGF0YTNAZW1haWwuY29t
                        > links/
                        > logs/
                            import-2022-01-01.log
                > 20/ba/
                    > 20bacbe5082d09eb3ac96a4565c1dc33
                        > 20bacbe5082d09eb3ac96a4565c1dc33.md5 << b64:ZGF0YTFAbmV0LmNvbQ==
                        > links/
                            > 5f4dcc3b5aa765d61d8327deb882cf99 -> ../../../../../password/5f/4d/5f4dcc3b5aa765d61d8327deb882cf99
                        > logs/
                            import.log << 20bacbe5082d09eb3ac96a4565c1dc33 2022-01-01 693ad1be-c353-4562-b12a-930f2ed43b79
                            import.log.1    # Old logs
                > 26/af/
                    > 26af7d285fa312aa2f8d3857d0f00af4
                        > 26af7d285fa312aa2f8d3857d0f00af4.md5 << b64:ZGF0YTRAZG9tYWluLmNvbQ==
                        > links/
                            > 5f4dcc3b5aa765d61d8327deb882cf99 -> ../../../../../password/90/28/90282e03043af181c985c9891c52c00f
                        > logs/
                            import.log << 26af7d285fa312aa2f8d3857d0f00af4 2022-01-01 693ad1be-c353-4562-b12a-930f2ed43b79
                            import.log.1    # Old logs
                > ae/68/
                    > ae68135e4f74eed19a79fd982c7c4f98
                        > ae68135e4f74eed19a79fd982c7c4f98.md5 << b64:ZGF0YTJAZW1haWwuY29t
                        > links/
                        > logs/
                            import.log << ae68135e4f74eed19a79fd982c7c4f98 2022-01-01 693ad1be-c353-4562-b12a-930f2ed43b79
                            import.log.1    # Old logs
                > xx/xx/
                    > lower_or_equal_8/
                    > lower_or_equal_10/
                    > lower_or_equal_12/
                    > more_than_12/
            > password/
                > _lower_or_equal_8/
                    > 5f4dcc3b5aa765d61d8327deb882cf99 -> ../5f/4d/5f4dcc3b5aa765d61d8327deb882cf99
                > _lower_or_equal_10/
                > _lower_or_equal_12/
                > _more_than_12/
                    > 90282e03043af181c985c9891c52c00f -> ../90/28/90282e03043af181c985c9891c52c00f
                > 5f/4d/
                    > 5f4dcc3b5aa765d61d8327deb882cf99/
                        > 5f4dcc3b5aa765d61d8327deb882cf99.md5 << b64:cGFzc3dvcmQ=
                        > links/
                            > 20bacbe5082d09eb3ac96a4565c1dc33 -> ../../../../../email/20/ba/20bacbe5082d09eb3ac96a4565c1dc33
                        > logs/
                            import.log << 5f4dcc3b5aa765d61d8327deb882cf99 2020-01-01 693ad1be-c353-4562-b12a-930f2ed43b79
                            import.log.1    # Old logs
                > 90/28/
                    > 90282e03043af181c985c9891c52c00f/
                        > 90282e03043af181c985c9891c52c00f.md5 << b64:YW5hd2Vzb21lcGFzc3dvcmQ=
                        > links/
                            > 26af7d285fa312aa2f8d3857d0f00af4 -> ../../../../../email/26/af/26af7d285fa312aa2f8d3857d0f00af4
                        > logs/
                            import.log << 90282e03043af181c985c9891c52c00f 2020-01-01 693ad1be-c353-4562-b12a-930f2ed43b79
                            import.log.1    # Old logs
                > xx/xx/
                
        > file
            > xx/xx/
            > 99/22/
                > 99226b116e370a25130cce7e55fe3f813a0f3168c30e584de422e9f43b76fc1a.sha256 << md5 8998c19bb14b9af66ccdc79bed5818c4 2022-01-01 << sha256 99226b116e370a25130cce7e55fe3f813a0f3168c30e584de422e9f43b76fc1a 2022-01-01 << status started << status done
            > xx/xx/
```

## Changelog

- **v2.x**: Builds graph at importation, calculate heuristics to reduce keyspace at partial search
- **v1.x**: Basic organization of data in filesystem