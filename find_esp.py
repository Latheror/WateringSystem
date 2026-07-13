import requests
import concurrent.futures

# Configuration
BASE_IP = "192.168.1."
PORT = 80
TIMEOUT = 1.0  # Seconds to wait for a response

def check_ip(ip):
    url = f"http://{BASE_IP}{ip}:{PORT}"
    try:
        # We use a head request to minimize data transfer, 
        # but some web servers might only respond to GET.
        # If HEAD fails, we can fallback to GET.
        response = requests.get(url, timeout=TIMEOUT)
        if response.status_code == 200:
            return ip, response.status_code
    except requests.exceptions.RequestException:
        pass
    return None

def main():
    print(f"Searching for ESP32 web app on {BASE_IP}0...")
    print("Scanning... this may take a moment.")
    
    found_ips = []
    
    # Use ThreadPoolExecutor to scan IPs in parallel for speed
    with concurrent.futures.ThreadPoolExecutor(max_workers=50) as executor:
        # Generate range of IPs from 1 to 254
        futures = {executor.submit(check_ip, str(i)): i for i in range(1, 255)}
        
        for future in concurrent.futures.as_completed(futures):
            result = future.result()
            if result:
                ip, status = result
                print(f"[+] Found device at {BASE_IP}{ip} (Status: {status})")
                found_ips.append(f"http://{BASE_IP}{ip}")

    if found_ips:
        print("\nFound the following web apps:")
        for url in found_ips:
            print(url)
    else:
        print("\nNo devices found on the local network.")

if __name__ == "__main__":
    main()
