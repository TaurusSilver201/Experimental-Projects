import speedtest

# Create a Speedtest object
st = speedtest.Speedtest()

# Get the user's internet download and upload speeds
download_speed = st.download() / 1e6  # Convert to Mbps
upload_speed = st.upload() / 1e6  # Convert to Mbps

# Print the results
print(f"Your download speed is: {download_speed:.2f} Mbps")
print(f"Your upload speed is: {upload_speed:.2f} Mbps")
