from bs4 import BeautifulSoup
import requests
import time
import smtplib
import os
import string
import urllib.request

url = "https://celestrak.com/NORAD/elements/stations.txt"
status_code = urllib.request.urlopen(url).getcode()
print(status_code)
    
#set headers
headers = {'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.95 Safari/537.36'}

# download the page
response = requests.get(url, headers=headers)

if status_code == 200:

    # parse the page
    soup = BeautifulSoup(response.text, "lxml")
    
    #print(soup)

    # Open and write the TLE data to the TLE.txt file
    f = open("TLE.txt", "w")

    # Grab text of TLE website:
    souptext = soup.get_text()

    # Write contents to TLE.txt file
    f.write(souptext)

    # Close the file 
    f.close()

    print("TLE data fetch was successful...")

else:
    print("There was an issue fetching the TLE data...")




