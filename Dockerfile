# ========== 阶段一：编译 Libs ==========

FROM ubuntu:24.04 AS builder

# 安装 Libs 所需的编译工具
RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    make \
    libssl-dev \
    liburing-dev \
    && rm -rf /var/lib/apt/lists/*

# 复制 Libs 源码
WORKDIR /build
COPY Libs/CMakeLists.txt /build/Libs/
COPY Libs/Net /build/Libs/Net
COPY Libs/publicShare /build/Libs/publicShare
COPY Libs/Simple /build/Libs/Simple

# 编译 Libs
WORKDIR /build/Libs/build
RUN cmake .. 
RUN make -j$(nproc)

# ========== 阶段二：编译 Server ==========
WORKDIR /build
COPY Server/CMakeLists.txt /build/Server/
COPY Server/source /build/Server/source
COPY Server/libs /build/Server/libs
COPY Server/ThirdParty /build/Server/ThirdParty

RUN mkdir -p /build/Server/bin
RUN cp /build/Libs/bin/*.so /build/Server/bin/

WORKDIR /build/Server/build
RUN cmake .. && make -j$(nproc)

# ========== 阶段三：运行 ==========
FROM ubuntu:24.04 AS runner

RUN apt-get update && apt-get install -y \
    libssl3 \
    liburing2 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app/bin

COPY --from=builder /build/Server/bin/GameService /app/bin/
COPY --from=builder /build/Server/bin/GameStateCenter /app/bin/
COPY --from=builder /build/Server/bin/UserFrontendService /app/bin/
COPY --from=builder /build/Server/bin/*.so /app/bin/
# COPY --from=builder /build/Libs/bin/EchoServerSimple /app/bin/


WORKDIR /app/config
COPY Server/config/*.ini /app/config/

# 设置动态链接库路径
ENV LD_LIBRARY_PATH=/app/bin:$LD_LIBRARY_PATH
