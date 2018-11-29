import crypt
import sys
salt, pwd = "8n./Hzqd", "This is my password!"
if len(sys.argv) == 3:
  _, salt, pwd = sys.argv
print crypt.crypt(pwd, "$6$%s" % salt)
