FROM gcc:10.2

RUN apt-get update -y
RUN apt-get install -y bc

RUN apt-get install -y libtbb-dev

COPY checker /apd/checker
COPY src /apd/src