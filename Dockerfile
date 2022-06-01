FROM emscripten/emsdk

RUN \
  emsdk update-tags && \
  emsdk install tot && \
  emsdk activate tot

# WORKDIR /app/

# ENTRYPOINT [ "./run.sh" ]