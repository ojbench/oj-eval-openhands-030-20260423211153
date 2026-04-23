

#ifndef SRC_HPP
#define SRC_HPP

#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>

typedef std::vector<std::vector<double>> IMAGE_T;

int judge(IMAGE_T &img);

// Helper functions for digit recognition
double calculateDensity(const IMAGE_T &img);
double calculateCenterOfMassX(const IMAGE_T &img);
double calculateCenterOfMassY(const IMAGE_T &img);
double calculateVerticalSymmetry(const IMAGE_T &img);
double calculateHorizontalSymmetry(const IMAGE_T &img);
int countHoles(const IMAGE_T &img);
double calculateTopDensity(const IMAGE_T &img);
double calculateBottomDensity(const IMAGE_T &img);
double calculateLeftDensity(const IMAGE_T &img);
double calculateRightDensity(const IMAGE_T &img);
bool hasVerticalStroke(const IMAGE_T &img);
bool hasHorizontalStroke(const IMAGE_T &img);
bool hasDiagonalStroke(const IMAGE_T &img);
double calculateAspectRatio(const IMAGE_T &img);

// Implementation of helper functions
double calculateDensity(const IMAGE_T &img) {
    double sum = 0;
    for (const auto &row : img) {
        for (double pixel : row) {
            sum += (1.0 - pixel); // Invert: 1 = black, 0 = white
        }
    }
    return sum / (img.size() * img[0].size());
}

double calculateCenterOfMassX(const IMAGE_T &img) {
    double totalMass = 0;
    double weightedX = 0;
    
    for (int y = 0; y < img.size(); y++) {
        for (int x = 0; x < img[y].size(); x++) {
            double mass = 1.0 - img[y][x]; // Invert: black pixels have mass
            totalMass += mass;
            weightedX += mass * x;
        }
    }
    
    return totalMass > 0 ? weightedX / totalMass : img[0].size() / 2.0;
}

double calculateCenterOfMassY(const IMAGE_T &img) {
    double totalMass = 0;
    double weightedY = 0;
    
    for (int y = 0; y < img.size(); y++) {
        for (int x = 0; x < img[y].size(); x++) {
            double mass = 1.0 - img[y][x]; // Invert: black pixels have mass
            totalMass += mass;
            weightedY += mass * y;
        }
    }
    
    return totalMass > 0 ? weightedY / totalMass : img.size() / 2.0;
}

double calculateVerticalSymmetry(const IMAGE_T &img) {
    double symmetry = 0;
    int width = img[0].size();
    int height = img.size();
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width / 2; x++) {
            double left = 1.0 - img[y][x];
            double right = 1.0 - img[y][width - 1 - x];
            symmetry += 1.0 - std::abs(left - right) / std::max(left + right, 0.001);
        }
    }
    
    return symmetry / (height * width / 2);
}

double calculateHorizontalSymmetry(const IMAGE_T &img) {
    double symmetry = 0;
    int width = img[0].size();
    int height = img.size();
    
    for (int y = 0; y < height / 2; y++) {
        for (int x = 0; x < width; x++) {
            double top = 1.0 - img[y][x];
            double bottom = 1.0 - img[height - 1 - y][x];
            symmetry += 1.0 - std::abs(top - bottom) / std::max(top + bottom, 0.001);
        }
    }
    
    return symmetry / (height / 2 * width);
}

int countHoles(const IMAGE_T &img) {
    // Simple hole counting based on enclosed white regions
    // This is a simplified approach
    int height = img.size();
    int width = img[0].size();
    std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
    int holes = 0;
    
    // Look for white regions that are surrounded by black pixels
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            if (img[y][x] > 0.7 && !visited[y][x]) { // White pixel
                bool isHole = true;
                // Check if this white region is surrounded
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        if (dy == 0 && dx == 0) continue;
                        if (y + dy >= 0 && y + dy < height && x + dx >= 0 && x + dx < width) {
                            if (img[y + dy][x + dx] < 0.3) { // Black neighbor
                                isHole = false;
                                break;
                            }
                        }
                    }
                    if (!isHole) break;
                }
                if (isHole) holes++;
            }
        }
    }
    
    return holes;
}

double calculateTopDensity(const IMAGE_T &img) {
    double sum = 0;
    int height = img.size();
    int width = img[0].size();
    int topRows = height / 3;
    
    for (int y = 0; y < topRows; y++) {
        for (int x = 0; x < width; x++) {
            sum += (1.0 - img[y][x]);
        }
    }
    
    return sum / (topRows * width);
}

double calculateBottomDensity(const IMAGE_T &img) {
    double sum = 0;
    int height = img.size();
    int width = img[0].size();
    int bottomRows = height / 3;
    
    for (int y = height - bottomRows; y < height; y++) {
        for (int x = 0; x < width; x++) {
            sum += (1.0 - img[y][x]);
        }
    }
    
    return sum / (bottomRows * width);
}

double calculateLeftDensity(const IMAGE_T &img) {
    double sum = 0;
    int height = img.size();
    int width = img[0].size();
    int leftCols = width / 3;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < leftCols; x++) {
            sum += (1.0 - img[y][x]);
        }
    }
    
    return sum / (height * leftCols);
}

double calculateRightDensity(const IMAGE_T &img) {
    double sum = 0;
    int height = img.size();
    int width = img[0].size();
    int rightCols = width / 3;
    
    for (int y = 0; y < height; y++) {
        for (int x = width - rightCols; x < width; x++) {
            sum += (1.0 - img[y][x]);
        }
    }
    
    return sum / (height * rightCols);
}

bool hasVerticalStroke(const IMAGE_T &img) {
    int height = img.size();
    int width = img[0].size();
    int centerCol = width / 2;
    
    // Check for continuous vertical stroke in center
    int continuousCount = 0;
    for (int y = 0; y < height; y++) {
        if (img[y][centerCol] < 0.3) { // Black pixel
            continuousCount++;
        } else {
            continuousCount = 0;
        }
        if (continuousCount > height / 3) return true;
    }
    
    return false;
}

bool hasHorizontalStroke(const IMAGE_T &img) {
    int height = img.size();
    int width = img[0].size();
    int centerRow = height / 2;
    
    // Check for continuous horizontal stroke in center
    int continuousCount = 0;
    for (int x = 0; x < width; x++) {
        if (img[centerRow][x] < 0.3) { // Black pixel
            continuousCount++;
        } else {
            continuousCount = 0;
        }
        if (continuousCount > width / 3) return true;
    }
    
    return false;
}

bool hasDiagonalStroke(const IMAGE_T &img) {
    int height = img.size();
    int width = img[0].size();
    
    // Check main diagonal
    int diagonalCount = 0;
    for (int i = 0; i < std::min(height, width); i++) {
        if (img[i][i] < 0.3) diagonalCount++;
    }
    
    return diagonalCount > std::min(height, width) / 4;
}

double calculateAspectRatio(const IMAGE_T &img) {
    int height = img.size();
    int width = img[0].size();
    
    // Find bounding box of the digit
    int minX = width, maxX = 0, minY = height, maxY = 0;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (img[y][x] < 0.3) { // Black pixel
                minX = std::min(minX, x);
                maxX = std::max(maxX, x);
                minY = std::min(minY, y);
                maxY = std::max(maxY, y);
            }
        }
    }
    
    if (maxX <= minX || maxY <= minY) return 1.0;
    
    return (double)(maxX - minX) / (maxY - minY);
}

// Main judge function
int judge(IMAGE_T &img) {
    // Extract features
    double density = calculateDensity(img);
    double centerX = calculateCenterOfMassX(img);
    double centerY = calculateCenterOfMassY(img);
    double vSymmetry = calculateVerticalSymmetry(img);
    double hSymmetry = calculateHorizontalSymmetry(img);
    int holes = countHoles(img);
    double topDensity = calculateTopDensity(img);
    double bottomDensity = calculateBottomDensity(img);
    double leftDensity = calculateLeftDensity(img);
    double rightDensity = calculateRightDensity(img);
    bool verticalStroke = hasVerticalStroke(img);
    bool horizontalStroke = hasHorizontalStroke(img);
    bool diagonalStroke = hasDiagonalStroke(img);
    double aspectRatio = calculateAspectRatio(img);
    
    // Rule-based classification
    std::vector<double> scores(10, 0);
    
    // Digit 0: High symmetry, one hole, circular
    if (holes >= 1 && vSymmetry > 0.7 && hSymmetry > 0.7) {
        scores[0] += 3;
    }
    
    // Digit 1: Vertical stroke, low density, centered
    if (verticalStroke && density < 0.15 && std::abs(centerX - img[0].size()/2.0) < 3) {
        scores[1] += 3;
    }
    
    // Digit 2: No holes, top-heavy, diagonal stroke
    if (holes == 0 && topDensity > bottomDensity && diagonalStroke) {
        scores[2] += 3;
    }
    
    // Digit 3: No holes, right-heavy, two semi-circles
    if (holes == 0 && rightDensity > leftDensity && hSymmetry < 0.5) {
        scores[3] += 3;
    }
    
    // Digit 4: High left density, vertical stroke, horizontal stroke
    if (leftDensity > rightDensity && verticalStroke && horizontalStroke) {
        scores[4] += 3;
    }
    
    // Digit 5: No holes, top-heavy, horizontal stroke
    if (holes == 0 && topDensity > bottomDensity && horizontalStroke) {
        scores[5] += 3;
    }
    
    // Digit 6: One hole, bottom-heavy
    if (holes >= 1 && bottomDensity > topDensity) {
        scores[6] += 3;
    }
    
    // Digit 7: Top-heavy, diagonal stroke
    if (topDensity > bottomDensity * 1.5 && diagonalStroke) {
        scores[7] += 3;
    }
    
    // Digit 8: Two holes, high symmetry
    if (holes >= 2 && vSymmetry > 0.6 && hSymmetry > 0.6) {
        scores[8] += 3;
    }
    
    // Digit 9: One hole, top-heavy
    if (holes >= 1 && topDensity > bottomDensity) {
        scores[9] += 3;
    }
    
    // Additional heuristics based on density and aspect ratio
    if (density < 0.1) {
        scores[1] += 1; // Very light, likely 1
    } else if (density > 0.25) {
        scores[8] += 1; // Very dense, likely 8 or 0
        scores[0] += 1;
    }
    
    if (aspectRatio > 1.2) {
        scores[7] += 1; // Wide, likely 7
    } else if (aspectRatio < 0.8) {
        scores[1] += 1; // Narrow, likely 1
    }
    
    // Find the digit with highest score
    int bestDigit = 0;
    double bestScore = scores[0];
    
    for (int i = 1; i < 10; i++) {
        if (scores[i] > bestScore) {
            bestScore = scores[i];
            bestDigit = i;
        }
    }
    
    // If no strong matches, use density-based fallback
    if (bestScore < 2) {
        if (density < 0.12) return 1;
        if (density < 0.18) return 7;
        if (density < 0.22) return 4;
        if (holes >= 1) return 0;
        return 8;
    }
    
    return bestDigit;
}

#endif

