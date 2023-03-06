# Hatofi DB

## What ?

Hatofi, stands for "Hashmap to Filesystem" is a key-value database running on Linux. Aims to reduce IO latency from traditional SQL database with less financial cost compared to Redis (stored in filesystem instead of RAM)

## Usage

- Database creation via config file via `hatofi --data_directory <data_dir> gen --config <config_file.txt>`
- Import data using `hatofi --data_directory <data_dir> load --input <input.txt>`
- Query data (partial or exact match) using `hatofi --data_directory <data_dir> query [--exact|--partial] <your_search>`

## Input Format

Input file format:
```txt
<entity_dataclass>:<entity_key>:<dataclass>:<base64_encoded_data>:<data_md5>
```
Assuming current date is `2022-01-01`

Assuming file UUIDv4 is `693ad1be-c353-4562-b12a-930f2ed43b79`

Example input.txt:
```txt
693ad1be-c353-4562-b12a-930f2ed43b79
email:20bacbe5082d09eb3ac96a4565c1dc33:email:ZGF0YTFAbmV0LmNvbQ==:20bacbe5082d09eb3ac96a4565c1dc33
email:20bacbe5082d09eb3ac96a4565c1dc33:password:cGFzc3dvcmQ=:5f4dcc3b5aa765d61d8327deb882cf99
email:ae68135e4f74eed19a79fd982c7c4f98:email:ZGF0YTJAZW1haWwuY29t:ae68135e4f74eed19a79fd982c7c4f98
email:1e77fd2c7a59f06a6c8dc8ace3ebf221:email:ZGF0YTNAZW1haWwuY29t:1e77fd2c7a59f06a6c8dc8ace3ebf221
email:26af7d285fa312aa2f8d3857d0f00af4:email:ZGF0YTRAZG9tYWluLmNvbQ==:26af7d285fa312aa2f8d3857d0f00af4
email:26af7d285fa312aa2f8d3857d0f00af4:password:YW5hd2Vzb21lcGFzc3dvcmQ=:90282e03043af181c985c9891c52c00f
```

```bash
$ md5sum input.txt 
3dca08a5fb0b47ae9a09e35c4fad9dbf  input.txt
$ sha256sum input.txt
9976ed3c6a2c2ced0002f2ffcbc4bb6bcaa6a653cd7f15846e187664e97159a2 input.txt
```

## Output Format

Desired filesystem output architecture:
```txt
> db/
    .dbmeta Database configuration file
    > data/
        > dataclasses/
            > email/
                > xx/xx/
                > 1e/77/
                    > 1e77fd2c7a59f06a6c8dc8ace3ebf221/
                        > 1e77fd2c7a59f06a6c8dc8ace3ebf221.md5 << md5:1e77fd2c7a59f06a6c8dc8ace3ebf221
                        > links/
                        > logs/
                            import-2022-01-01.log
                > 20/ba/
                    > 20bacbe5082d09eb3ac96a4565c1dc33
                        > 20bacbe5082d09eb3ac96a4565c1dc33.md5 << md5:20bacbe5082d09eb3ac96a4565c1dc33
                        > links/
                            > 5f4dcc3b5aa765d61d8327deb882cf99 -> ../../../../../password/5f/4d/5f4dcc3b5aa765d61d8327deb882cf99
                        > logs/
                            import.log << 20bacbe5082d09eb3ac96a4565c1dc33 2022-01-01 693ad1be-c353-4562-b12a-930f2ed43b79
                            import.log.1    # Old logs
                > 26/af/
                    > 26af7d285fa312aa2f8d3857d0f00af4
                        > 26af7d285fa312aa2f8d3857d0f00af4.md5 << md5:26af7d285fa312aa2f8d3857d0f00af4
                        > links/
                            > 5f4dcc3b5aa765d61d8327deb882cf99 -> ../../../../../password/90/28/90282e03043af181c985c9891c52c00f
                        > logs/
                            import.log << 26af7d285fa312aa2f8d3857d0f00af4 2022-01-01 693ad1be-c353-4562-b12a-930f2ed43b79
                            import.log.1    # Old logs
                > ae/68/
                    > ae68135e4f74eed19a79fd982c7c4f98
                        > ae68135e4f74eed19a79fd982c7c4f98.md5 << md5:ae68135e4f74eed19a79fd982c7c4f98
                        > links/
                        > logs/
                            import.log << ae68135e4f74eed19a79fd982c7c4f98 2022-01-01 693ad1be-c353-4562-b12a-930f2ed43b79
                            import.log.1    # Old logs
                > xx/xx/
            > password/
                > 5f/4d/
                    > 5f4dcc3b5aa765d61d8327deb882cf99/
                        > 5f4dcc3b5aa765d61d8327deb882cf99.md5 << md5:5f4dcc3b5aa765d61d8327deb882cf99
                        > links/
                            > 20bacbe5082d09eb3ac96a4565c1dc33 -> ../../../../../email/20/ba/20bacbe5082d09eb3ac96a4565c1dc33
                        > logs/
                            import.log << 5f4dcc3b5aa765d61d8327deb882cf99 2020-01-01 693ad1be-c353-4562-b12a-930f2ed43b79
                            import.log.1    # Old logs
                > 90/28/
                    > 90282e03043af181c985c9891c52c00f/
                        > 90282e03043af181c985c9891c52c00f.md5 << md5:90282e03043af181c985c9891c52c00f
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

**MAIN CHANGE OF 2.1: Removed support for full-text search optimization. Will be delegated to an external project.**

## Changelog

- **2.1.x**: Remove support for partial data search: Database is now full anonymized
- **v2.x**: Builds graph at importation, calculate heuristics to reduce keyspace at partial search
- **v1.x**: Basic organization of data in filesystem