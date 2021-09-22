FROM debian
ENV TZ=America/New_York
ENV DEBIAN_FRONTEND=noninteractive
RUN apt update
RUN apt install -y gcc g++ libpqxx-dev libpoco-dev cmake
COPY . . 
RUN mkdir build
WORKDIR build
RUN cmake ..
RUN cmake --build . --config Release
CMD ["./CryptoBrokerCollector"]

