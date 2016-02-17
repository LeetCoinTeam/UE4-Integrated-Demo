#!/usr/bin/python
# LEET get server credentials by serverKey and game credentials
# This is called on server creation to fetch the serverKey and serverSecret.  
# The Key and secret get stored in the UE4 Game.ini config file.
# args:  serverKey gameAPISecret gameAPIKey
import sys
import time
import json
import hashlib
import hmac
import urllib
import httplib

total = len(sys.argv)
 
# Get the arguments list 
cmdargs = str(sys.argv)
 
url="apitest-dot-leetsandbox.appspot.com"
uri = "/api/v2/server/credentials" 
params = {
          "nonce": time.time(),
          "serverKey": str(sys.argv[1]),
                  }
params = urllib.urlencode(params)
# Hash the params string to produce the Sign header value
H = hmac.new(str(sys.argv[2]), digestmod=hashlib.sha512)
H.update(params)
sign = H.hexdigest()
headers = {"Content-type": "application/x-www-form-urlencoded",
                           "Key":str(sys.argv[3]),
                           "Sign":sign}
conn = httplib.HTTPSConnection(url)
conn.request("POST", uri, params, headers)
response = conn.getresponse()
#print (response.read())
response_data = json.loads(response.read())
print (response_data['serverAPIKey'])
print (response_data['serverAPISecret'])
f = open('Game.ini','w')
f.write('[LEET.Server]\n')
f.write('ServerAPIKey=%s\n'%response_data['serverAPIKey'])
f.write('ServerAPISecret=%s\n'%response_data['serverAPISecret'])
f.close()