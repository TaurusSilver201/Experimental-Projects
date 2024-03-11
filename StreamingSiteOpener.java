import java.awt.*;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.ArrayList;
import java.util.List;
import javax.swing.*;
import javax.swing.filechooser.FileNameExtensionFilter;

/**
 * This class provides an advanced utility to open major streaming sites and custom URLs
 * in the default web browser. It includes a graphical user interface (GUI) with progress
 * tracking, error logging, and the ability to import URLs from a file.
 */
public class StreamingSiteOpener extends JFrame {
    private static final Desktop desktop;
    private static final List<String> streamingSites;
    private JTextArea urlTextArea;
    private JProgressBar progressBar;
    private JButton openButton, importButton;

    static {
        desktop = Desktop.isDesktopSupported() ? Desktop.getDesktop() : null;
        streamingSites = new ArrayList<>();
        streamingSites.add("https://www.netflix.com");
        streamingSites.add("https://www.hulu.com");
        streamingSites.add("https://www.primevideo.com");
        streamingSites.add("https://www.disneyplus.com");
        streamingSites.add("https://www.hbomax.com");
        streamingSites.add("https://www.paramountplus.com");
        streamingSites.add("https://www.peacocktv.com");
        streamingSites.add("https://www.appletv.apple.com");
    }

    public StreamingSiteOpener() {
        setTitle("Streaming Site Opener");
        setSize(600, 400);
        setLocationRelativeTo(null);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setLayout(new BorderLayout());

        JPanel inputPanel = new JPanel(new BorderLayout());
        urlTextArea = new JTextArea(10, 30);
        inputPanel.add(new JScrollPane(urlTextArea), BorderLayout.CENTER);

        JPanel buttonPanel = new JPanel();
        openButton = new JButton("Open URLs");
        openButton.addActionListener(e -> openURLs());
        buttonPanel.add(openButton);

        importButton = new JButton("Import URLs from File");
        importButton.addActionListener(e -> importURLsFromFile());
        buttonPanel.add(importButton);

        inputPanel.add(buttonPanel, BorderLayout.SOUTH);
        add(inputPanel, BorderLayout.NORTH);

        progressBar = new JProgressBar();
        add(progressBar, BorderLayout.SOUTH);

        setVisible(true);
    }

    /**
     * Opens the URLs specified in the text area in the default web browser.
     */
    private void openURLs() {
        String[] urlsArray = urlTextArea.getText().split("\\s+");
        List<String> urls = new ArrayList<>(urlsArray.length + streamingSites.size());
        for (String url : urlsArray) {
            urls.add(url);
        }
        urls.addAll(streamingSites);

        int totalUrls = urls.size();
        progressBar.setMaximum(totalUrls);
        progressBar.setValue(0);

        int openedUrls = 0;
        for (String url : urls) {
            if (openStreamingSite(url)) {
                openedUrls++;
            }
            progressBar.setValue(openedUrls);
        }

        if (openedUrls == totalUrls) {
            JOptionPane.showMessageDialog(this, "All URLs opened successfully.");
        } else {
            JOptionPane.showMessageDialog(this, "Error opening some URLs. Check the logs for details.", "Error", JOptionPane.ERROR_MESSAGE);
        }
    }

    /**
     * Opens the given streaming site URL in the default web browser.
     *
     * @param siteUrl The URL of the streaming site to open.
     * @return true if the site was opened successfully, false otherwise.
     */
    private boolean openStreamingSite(String siteUrl) {
        if (desktop == null) {
            logError("Desktop is not supported on this platform.");
            return false;
        }

        try {
            desktop.browse(new URI(siteUrl));
            return true;
        } catch (IOException | URISyntaxException e) {
            logError("Failed to open: " + siteUrl);
            e.printStackTrace();
            return false;
        }
    }

    /**
     * Imports URLs from a text file and populates the text area.
     */
    private void importURLsFromFile() {
        JFileChooser fileChooser = new JFileChooser();
        FileNameExtensionFilter filter = new FileNameExtensionFilter("Text Files", "txt");
        fileChooser.setFileFilter(filter);

        int result = fileChooser.showOpenDialog(this);
        if (result == JFileChooser.APPROVE_OPTION) {
            File file = fileChooser.getSelectedFile();
            try (BufferedReader reader = new BufferedReader(new FileReader(file))) {
                StringBuilder urlsBuilder = new StringBuilder();
                String line;
                while ((line = reader.readLine()) != null) {
                    urlsBuilder.append(line).append(" ");
                }
                urlTextArea.setText(urlsBuilder.toString());
            } catch (IOException e) {
                JOptionPane.showMessageDialog(this, "Error reading file: " + e.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
            }
        }
    }

    /**
     * Logs an error message to the console.
     *
     * @param message The error message to log.
     */
    private static void logError(String message) {
        System.err.println(message);
    }

    /**
     * The main entry point of the program.
     *
     * @param args The command-line arguments.
     */
    public static void main(String[] args) {
        if (args.length > 0) {
            for (String arg : args) {
                openStreamingSite(arg);
            }
        } else {
            SwingUtilities.invokeLater(StreamingSiteOpener::new);
        }
    }
}
