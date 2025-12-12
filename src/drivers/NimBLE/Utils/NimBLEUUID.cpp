# include "NimBLEUUID.h"

NimBLEUUID::NimBLEUUID(const char *UUID) {
  if (UUID == nullptr) {
    reset();
    return;
  }
  constexpr int UUID_LEN = sizeof(mUUID.value) / sizeof(mUUID.value[0]); //compile-time вычисление размера массива
  if (sscanf(UUID, 
       "%2hhx%2hhx%2hhx%2hhx-"
       "%2hhx%2hhx-"
       "%2hhx%2hhx-"
       "%2hhx%2hhx-"
       "%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",

       &mUUID.value[15], &mUUID.value[14], &mUUID.value[13], &mUUID.value[12],  // Обратный порядок!
       &mUUID.value[11], &mUUID.value[10],
       &mUUID.value[9], &mUUID.value[8],
       &mUUID.value[7], &mUUID.value[6],
       &mUUID.value[5], &mUUID.value[4], &mUUID.value[3], &mUUID.value[2], &mUUID.value[1], &mUUID.value[0]) != UUID_LEN) {
    reset();
  }
}

NimBLEUUID& NimBLEUUID::operator = (const char* UUID) {
  if (UUID == nullptr) {
    reset();
    return *this;
  }  
  constexpr int UUID_LEN = sizeof(mUUID.value) / sizeof(mUUID.value[0]); //compile-time вычисление размера массива
  if (sscanf(UUID, 
       "%2hhx%2hhx%2hhx%2hhx-"
       "%2hhx%2hhx-"
       "%2hhx%2hhx-"
       "%2hhx%2hhx-"
       "%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",

       &mUUID.value[15], &mUUID.value[14], &mUUID.value[13], &mUUID.value[12], 
       &mUUID.value[11], &mUUID.value[10],
       &mUUID.value[9], &mUUID.value[8],
       &mUUID.value[7], &mUUID.value[6],
       &mUUID.value[5], &mUUID.value[4], &mUUID.value[3], &mUUID.value[2], &mUUID.value[1], &mUUID.value[0]) != UUID_LEN) {
    reset();
  }
  return *this;
}

NimBLEUUID::NimBLEUUID(const NimBLEUUID& uuid) {
  constexpr int UUID_LEN = sizeof(mUUID.value) / sizeof(mUUID.value[0]); //compile-time вычисление размера массива
  for (int i = 0; i < UUID_LEN; i++) {
    mUUID.value[i] = uuid.mUUID.value[i];
  }
}

NimBLEUUID::NimBLEUUID(const NimBLEUUID&& uuid) {
  constexpr int UUID_LEN = sizeof(mUUID.value) / sizeof(mUUID.value[0]); //compile-time вычисление размера массива
  for (int i = 0; i < UUID_LEN; i++) {
    mUUID.value[i] = uuid.mUUID.value[i];
  }
}

NimBLEUUID& NimBLEUUID::operator = (const NimBLEUUID& uuid) {
  constexpr int UUID_LEN = sizeof(mUUID.value) / sizeof(mUUID.value[0]); //compile-time вычисление размера массива
  for (int i = 0; i < UUID_LEN; i++) {
    mUUID.value[i] = uuid.mUUID.value[i];
  }
  return *this;
}

NimBLEUUID& NimBLEUUID::operator = (const NimBLEUUID&& uuid) {
  constexpr int UUID_LEN = sizeof(mUUID.value) / sizeof(mUUID.value[0]); //compile-time вычисление размера массива
  for (int i = 0; i < UUID_LEN; i++) {
    mUUID.value[i] = uuid.mUUID.value[i];
  }
  return *this;
}