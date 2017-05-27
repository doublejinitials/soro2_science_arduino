#ifndef SERIALIZE_H
#define SERIALIZE_H

/* Serializes numeric data into a char array
 */
template <typename T>
inline void serialize(char *arr, T data) {
  int max = sizeof(T) - 1;
  for (int i = 0; i <= max; i++) {
    arr[max - i] = (data >> (i * 8)) & 0xFF;
  }
}

/* Deseralizes numeric data from a char array to its original value
 */
template <typename T>
inline T deserialize(const char *arr) {
  T result;
  for (unsigned int i = 0; i < sizeof(T); i++) {
    result = (result << 8) + reinterpret_cast<const unsigned char&>(arr[i]);
  }
  return result;
}

#endif
