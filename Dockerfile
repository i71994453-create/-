FROM python:3.12-slim

ENV PYTHONDONTWRITEBYTECODE=1 \
    PYTHONUNBUFFERED=1 \
    GPAI_LIBRARY=/app/libgpai_core.so \
    PYTHONPATH=/app/backend

WORKDIR /app
RUN apt-get update \
    && apt-get install -y --no-install-recommends g++ \
    && rm -rf /var/lib/apt/lists/*

COPY backend/requirements.txt /app/backend/requirements.txt
RUN pip install --no-cache-dir -r /app/backend/requirements.txt

COPY cpp/gpai_core.cpp cpp/gpai_core.h cpp/gpai_calculations.hpp cpp/gpai_dispatch.cpp cpp/gpai_engine.cpp cpp/gpai_engine.h /app/cpp/
RUN g++ -std=c++17 -O2 -fPIC -shared -DGPAI_CORE_BUILD -I/app/cpp \
    /app/cpp/gpai_core.cpp /app/cpp/gpai_dispatch.cpp /app/cpp/gpai_engine.cpp -o /app/libgpai_core.so

COPY backend/ /app/backend/
EXPOSE 8080
CMD ["sh", "-c", "uvicorn app:app --host 0.0.0.0 --port ${PORT:-8080}"]
