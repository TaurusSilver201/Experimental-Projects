import net.sourceforge.tess4j.Tesseract;
import net.sourceforge.tess4j.TesseractException;
import net.sourceforge.tess4j.Word;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.List;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.logging.Level;
import java.util.logging.Logger;

public class ImageToTextExtractor {
    private static final Logger logger = Logger.getLogger(ImageToTextExtractor.class.getName());
    private static final AtomicInteger processedCount = new AtomicInteger();
    private static final AtomicInteger errorCount = new AtomicInteger();
    private static final ExecutorService executor = Executors.newFixedThreadPool(Runtime.getRuntime().availableProcessors());
    private static final BlockingQueue<Path> imageQueue = new LinkedBlockingQueue<>();

    public static void main(String[] args) {
        String imagePath = "/path/to/images";
        String outputPath = "/path/to/output";
        String dataPath = "/path/to/tessdata";
        String language = "eng"; // Set the language code for OCR

        try {
            // Start worker threads
            for (int i = 0; i < Runtime.getRuntime().availableProcessors(); i++) {
                executor.execute(new ImageProcessingWorker(outputPath, dataPath, language));
            }

            // Traverse the input directory and add files to the queue
            Files.walk(Paths.get(imagePath))
                 .filter(path -> path.toString().endsWith(".png") || path.toString().endsWith(".jpg"))
                 .forEach(path -> {
                     try {
                         imageQueue.put(path);
                     } catch (InterruptedException e) {
                         logger.log(Level.SEVERE, "Error occurred while adding file to queue: " + path, e);
                         Thread.currentThread().interrupt();
                     }
                 });

            // Poison pill to signal workers to terminate
            for (int i = 0; i < Runtime.getRuntime().availableProcessors(); i++) {
                imageQueue.put(null);
            }

            // Wait for all tasks to complete
            executor.shutdown();
            boolean terminated = executor.awaitTermination(Long.MAX_VALUE, TimeUnit.NANOSECONDS);

            if (terminated) {
                logger.info("Text extraction completed. Processed " + processedCount.get() + " files.");
                if (errorCount.get() > 0) {
                    logger.severe("Errors occurred during processing " + errorCount.get() + " files.");
                }
            } else {
                logger.severe("Text extraction was interrupted.");
            }
        } catch (IOException | InterruptedException e) {
            logger.log(Level.SEVERE, "Error occurred: " + e.getMessage(), e);
        }
    }

    private static class ImageProcessingWorker implements Runnable {
        private final String outputDir;
        private final String dataPath;
        private final String language;

        public ImageProcessingWorker(String outputDir, String dataPath, String language) {
            this.outputDir = outputDir;
            this.dataPath = dataPath;
            this.language = language;
        }

        @Override
        public void run() {
            Tesseract tesseract = new Tesseract();
            tesseract.setDatapath(dataPath);
            tesseract.setLanguage(language);
            tesseract.setPageSegMode(1); // Automatic page segmentation with OSD
            tesseract.setOcrEngineMode(3); // Enable only word recognition

            while (true) {
                Path imagePath;
                try {
                    imagePath = imageQueue.take();
                    if (imagePath == null) {
                        break; // Poison pill received, terminate worker
                    }
                } catch (InterruptedException e) {
                    logger.log(Level.SEVERE, "Worker thread interrupted", e);
                    Thread.currentThread().interrupt();
                    break;
                }

                try {
                    File imageFile = imagePath.toFile();
                    BufferedImage image = ImageIO.read(imageFile);
                    List<Word> words = tesseract.getWords(image);

                    String outputFileName = imagePath.getFileName().toString().replace(".png", ".txt").replace(".jpg", ".txt");
                    String outputFilePath = Paths.get(outputDir, outputFileName).toString();
                    Files.write(Paths.get(outputFilePath), extractTextFromWords(words).getBytes());

                    processedCount.incrementAndGet();
                    logger.info("Processed: " + imagePath);
                } catch (TesseractException | IOException e) {
                    errorCount.incrementAndGet();
                    logger.log(Level.SEVERE, "Error occurred while processing " + imagePath, e);
                }
            }
        }

        private String extractTextFromWords(List<Word> words) {
            StringBuilder extractedText = new StringBuilder();
            for (Word word : words) {
                extractedText.append(word.getText()).append(" ");
            }
            return extractedText.toString().trim();
        }
    }
}
