import requests
def send_simple_message(name,mailid,message):
    return requests.post(
        "<api_url>",
        auth=("api", "<api_key>"),
        data={"from": "<mail_id>",
            "to": [""+name+" <"+mailid+">"],
            "subject": "Prescription",
            "text": ""+message+""})

print send_simple_message()