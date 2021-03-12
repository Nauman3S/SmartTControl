import math,re
from datetime import datetime

# Current date time in local system
print(datetime.now())


def is_number(s):
    try:
        float(s)
        return True
    except ValueError:
        return False



# t='1-'

# print(is_number(t))


# s = 'asdf=5;iwantthis123jasd'
# result = re.search('asdf=5;(.*)123jasd', s)
# print(result.group(1))

# gg=s.index('iwant')
# print(s.index('iwant'))
# print(s[gg:gg+3])

# mm='id_707_t'
# print(int(mm))