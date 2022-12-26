# Hatofi DB

## Input Format

Input file format:
```txt
<entity_key>:<dataclass>:<base64_encoded_data>:<data_md5>
```
Assuming current date is `2022-01-01`
Assuming leak UUIDv4 is `693ad1be-c353-4562-b12a-930f2ed43b79`

Example input.txt:
```txt
693ad1be-c353-4562-b12a-930f2ed43b79
20bacbe5082d09eb3ac96a4565c1dc33:email:ZGF0YTFAbmV0LmNvbQ==:20bacbe5082d09eb3ac96a4565c1dc33
20bacbe5082d09eb3ac96a4565c1dc33:password:cGFzc3dvcmQ=:5f4dcc3b5aa765d61d8327deb882cf99
ae68135e4f74eed19a79fd982c7c4f98:email:ZGF0YTJAZW1haWwuY29t:ae68135e4f74eed19a79fd982c7c4f98
1e77fd2c7a59f06a6c8dc8ace3ebf221:email:ZGF0YTNAZW1haWwuY29t:1e77fd2c7a59f06a6c8dc8ace3ebf221
26af7d285fa312aa2f8d3857d0f00af4:email:ZGF0YTRAZG9tYWluLmNvbQ==:26af7d285fa312aa2f8d3857d0f00af4
26af7d285fa312aa2f8d3857d0f00af4:password:YW5hd2Vzb21lcGFzc3dvcmQ=:90282e03043af181c985c9891c52c00f
```

## Output Format

Desired filesystem output architecture:
```txt
> db/
    .meta Database configuration file
    > data/
        > dataclasses/
            > email/
                > xx/xx/
                > 1e/77/
                    > 1e77fd2c7a59f06a6c8dc8ace3ebf221.md5 << b64:ZGF0YTNAZW1haWwuY29t
                > 20/ba/
                    > 20bacbe5082d09eb3ac96a4565c1dc33.md5 << b64:ZGF0YTFAbmV0LmNvbQ==
                > 26/af/
                    > 26af7d285fa312aa2f8d3857d0f00af4.md5 << b64:ZGF0YTRAZG9tYWluLmNvbQ==
                > ae/68/
                    > ae68135e4f74eed19a79fd982c7c4f98.md5 << b64:ZGF0YTJAZW1haWwuY29t
                > xx/xx/
            > password/
                > xx/xx/
                > 5f/4d/
                    > 5f4dcc3b5aa765d61d8327deb882cf99.md5 << b64:cGFzc3dvcmQ=
                > 90/28/
                    > 90282e03043af181c985c9891c52c00f.md5 << b64:YW5hd2Vzb21lcGFzc3dvcmQ=
                > xx/xx/
        > entity
            > xx/xx/
            > 1e/77/
                > 1e77fd2c7a59f06a6c8dc8ace3ebf221.md5 << 1e77fd2c7a59f06a6c8dc8ace3ebf221 2022-01-01 693ad1be-c353-4562-b12a-930f2ed43b79
            > 20/ba/
                > 20bacbe5082d09eb3ac96a4565c1dc33.md5 << 20bacbe5082d09eb3ac96a4565c1dc33 2022-01-01 693ad1be-c353-4562-b12a-930f2ed43b79 << 5f4dcc3b5aa765d61d8327deb882cf99 2022-01-01 693ad1be-c353-4562-b12a-930f2ed43b79
            > 26/af/
                > 26af7d285fa312aa2f8d3857d0f00af4.md5 << 26af7d285fa312aa2f8d3857d0f00af4 2022-01-01 693ad1be-c353-4562-b12a-930f2ed43b79 << 90282e03043af181c985c9891c52c00f 2022-01-01 693ad1be-c353-4562-b12a-930f2ed43b79
            > ae/68/
                > ae68135e4f74eed19a79fd982c7c4f98.md5 << ae68135e4f74eed19a79fd982c7c4f98 2022-01-01 693ad1be-c353-4562-b12a-930f2ed43b79
            > xx/xx/
```