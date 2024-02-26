import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

public class AcademicBookSearch {

    public static List<String> getAcademicBooks(String subject) {
        List<String> books = new ArrayList<>();
        try {
            String searchQuery = "academic books on " + URLEncoder.encode(subject, "UTF-8");
            String searchUrl = "https://www.google.com/search?q=" + searchQuery;
            Document doc = Jsoup.connect(searchUrl).userAgent("Mozilla").get();
            Elements results = doc.select("h3");
            for (Element result : results) {
                books.add(result.text());
            }
        } catch (UnsupportedEncodingException | IOException e) {
            e.printStackTrace();
        }
        return books;
    }

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        System.out.print("Enter a subject: ");
        String subject = scanner.nextLine();
        List<String> academicBooks = getAcademicBooks(subject);
        if (!academicBooks.isEmpty()) {
            System.out.println("Suggested academic books on " + subject + ":");
            for (String book : academicBooks) {
                System.out.println(book);
            }
        } else {
            System.out.println("No academic books found on " + subject + ".");
        }
        scanner.close();
    }
}
