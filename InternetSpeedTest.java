import java.io.BufferedInputStream;
import java.io.IOException;
import java.net.URL;
import java.net.URLConnection;

public class InternetSpeedTest {
    private static final String TEST_FILE_URL = "http://ipv4.ikoula.testdebit.info/1G.rem";
    private static final int FILE_SIZE = 1000 * 1000 * 1000; // 1 GB

    public static void main(String[] args) {
        long startTime = System.currentTimeMillis();
        try {
            URL url = new URL(TEST_FILE_URL);
            URLConnection connection = url.openConnection();
            BufferedInputStream in = new BufferedInputStream(connection.getInputStream());

            byte[] data = new byte[1024];
            int bytesRead;
            long totalBytesRead = 0;

            while ((bytesRead = in.read(data, 0, 1024)) != -1) {
                totalBytesRead += bytesRead;
            }

            in.close();

            long endTime = System.currentTimeMillis();
            long downloadDuration = endTime - startTime;
            long downloadSpeed = (FILE_SIZE / (downloadDuration / 1000)) / 1000; // Kilobits per second

            System.out.println("Download Speed: " + downloadSpeed + " Kbps");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
