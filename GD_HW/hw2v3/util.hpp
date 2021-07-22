#pragma once

#define clearVector(v) \
  for(auto v##_el : v) delete v##_el; \
  v.clear()

#define clearQueue(q) \
  while (!q.empty()) { \
    delete q.front(); \
    q.pop(); \
  }

#define removeElement(v, el, ...)  \
for (auto $ = v.begin(); $ != v.end(); $++) { \
  if (*$ == el) { \
    v.erase($); \
    __VA_ARGS__ \
  } \
}
