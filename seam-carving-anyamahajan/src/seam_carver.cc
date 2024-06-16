#include "seam_carver.hpp"

#include "image_ppm.hpp"

// implement the rest of SeamCarver's functions here

// given functions below, DO NOT MODIFY

SeamCarver::SeamCarver(const ImagePPM& image): image_(image) {
  height_ = image.GetHeight();
  width_ = image.GetWidth();
}

void SeamCarver::SetImage(const ImagePPM& image) {
  image_ = image;
  width_ = image.GetWidth();
  height_ = image.GetHeight();
}

const ImagePPM& SeamCarver::GetImage() const { return image_; }

int SeamCarver::GetHeight() const { return height_; }
int SeamCarver::GetWidth() const { return width_; }
int SeamCarver::GetEnergy(int row, int col) const {
  row = (row + height_) % height_;
  col = (col + width_) % width_;
  int temp_top = ((row - 1) + height_) % height_;
  int temp_bottom = ((row + 1) % height_);
  int temp_left = ((col - 1) + width_) % width_;
  int temp_right = ((col + 1) % width_);
  Pixel top = image_.GetPixel(temp_top, col);
  Pixel bottom = image_.GetPixel(temp_bottom, col);
  Pixel left = image_.GetPixel(row, temp_left);
  Pixel right = image_.GetPixel(row, temp_right);
  int total_energy = 0;
  int row_red_energy =
      (top.GetRed() - bottom.GetRed()) * (top.GetRed() - bottom.GetRed());
  int row_green_energy = (top.GetGreen() - bottom.GetGreen()) *
                         (top.GetGreen() - bottom.GetGreen());
  int row_blue_energy =
      (top.GetBlue() - bottom.GetBlue()) * (top.GetBlue() - bottom.GetBlue());
  int col_red_energy =
      (right.GetRed() - left.GetRed()) * (right.GetRed() - left.GetRed());
  int col_green_energy = (right.GetGreen() - left.GetGreen()) *
                         (right.GetGreen() - left.GetGreen());
  int col_blue_energy =
      (right.GetBlue() - left.GetBlue()) * (right.GetBlue() - left.GetBlue());
  total_energy = row_red_energy + row_green_energy + row_blue_energy +
                 col_red_energy + col_green_energy + col_blue_energy;
  return total_energy;
}
int SeamCarver::GetMinIndexHorizontal(int** energy_vals) const {
  for (int row = 0; row < height_; row++) {
    energy_vals[row][width_ - 1] = GetEnergy(row, width_ - 1);
  }
  for (int col = width_ - 2; col >= 0; col--) {
    for (int row = 0; row < height_; row++) {
      if (height_ == 1) {
        energy_vals[row][col] = energy_vals[row][col + 1] + GetEnergy(row, col);
      } else if (row == 0) {
        energy_vals[row][col] =
            std::min(energy_vals[row][col + 1], energy_vals[row + 1][col + 1]) +
            GetEnergy(row, col);
      } else if (row == height_ - 1) {
        energy_vals[row][col] =
            std::min(energy_vals[row][col + 1], energy_vals[row - 1][col + 1]) +
            GetEnergy(row, col);
      } else {
        energy_vals[row][col] =
            std::min((energy_vals[row - 1][col + 1]),
                     std::min(energy_vals[row][col + 1],
                              energy_vals[row + 1][col + 1])) +
            GetEnergy(row, col);
      }
    }
  }
  int min_index = 0, min = INT32_MAX;
  for (int row = 0; row < height_; row++) {
    if (energy_vals[row][0] < min) {
      min = energy_vals[row][0];
      min_index = row;
    }
  }
  return min_index;
}
int* SeamCarver::GetHorizontalSeam() const {
  int** energy_vals = new int*[height_];
  for (int row = 0; row < height_; row++) {
    energy_vals[row] = new int[width_];
  }

  int* horizontal_seam = new int[width_];
  int row = GetMinIndexHorizontal(energy_vals);
  int col = 0;
  horizontal_seam[0] = row;
  for (int index = 1; index < width_; index++) {
    if (row == 0) {
      if (energy_vals[row + 1][col + 1] < energy_vals[row][col + 1]) {
        row += 1;
      }
    } else if (row == height_ - 1) {
      if (energy_vals[row - 1][col + 1] < energy_vals[row][col + 1]) {
        row -= 1;
      }
    } else {
      if ((energy_vals[row - 1][col + 1] < energy_vals[row][col + 1]) &&
          energy_vals[row - 1][col + 1] <= energy_vals[row + 1][col + 1]) {
        row -= 1;
      } else {
        if (energy_vals[row + 1][col + 1] < energy_vals[row][col + 1]) {
          row += 1;
        }
      }
    }
    horizontal_seam[index] = row;
    col = col + 1;
  }
  for (int row = 0; row < height_; ++row) {
    delete[] energy_vals[row];
  }
  delete[] energy_vals;
  return horizontal_seam;
}
int SeamCarver::GetMinIndexVertical(int** energy_vals) const {
  std::cout << "start" << std::endl;
  for (int col = 0; col < width_; col++) {
    energy_vals[height_ - 1][col] = GetEnergy(height_ - 1, col);
  }
  std::cout << "middle" << std::endl;
  for (int row = height_ - 2; row >= 0; row--) {
    for (int col = 0; col < width_; col++) {
      if (width_ == 1) {
        energy_vals[row][col] = energy_vals[row + 1][col] + GetEnergy(row, col);
      } else if (col == 0) {
        energy_vals[row][col] =
            std::min(energy_vals[row + 1][col], energy_vals[row + 1][col + 1]) +
            GetEnergy(row, col);
      } else if (col == width_ - 1) {
        energy_vals[row][col] =
            std::min(energy_vals[row + 1][col - 1], energy_vals[row + 1][col]) +
            GetEnergy(row, col);
      } else {
        energy_vals[row][col] =
            std::min(energy_vals[row + 1][col - 1],
                     std::min(energy_vals[row + 1][col],
                              energy_vals[row + 1][col + 1])) +
            GetEnergy(row, col);
      }
    }
  }
  int min_index = 0;
  int min = INT32_MAX;
  for (int col = 0; col < width_; col++) {
    if (energy_vals[0][col] < min) {
      min = energy_vals[0][col];
      min_index = col;
    }
  }
  return min_index;
}
int* SeamCarver::GetVerticalSeam() const {
  int** energy_vals = new int*[height_];
  for (int row = 0; row < height_; row++) {
    energy_vals[row] = new int[width_];
  }

  int* vertical_seam = new int[height_];
  int col = GetMinIndexVertical(energy_vals);
  int row = 0;
  vertical_seam[0] = col;
  for (int index = 1; index < height_; index++) {
    if (col == 0) {
      if (energy_vals[row + 1][col + 1] < energy_vals[row + 1][col]) {
        col += 1;
      }
    } else if (col == width_ - 1) {
      if (energy_vals[row + 1][col - 1] < energy_vals[row + 1][col]) {
        col -= 1;
      }
    } else {
      if ((energy_vals[row + 1][col - 1] < energy_vals[row + 1][col]) &&
          energy_vals[row + 1][col - 1] <= energy_vals[row + 1][col + 1]) {
        col -= 1;
      } else {
        if (energy_vals[row + 1][col + 1] < energy_vals[row + 1][col]) {
          col += 1;
        }
      }
    }
    vertical_seam[index] = col;
    row += 1;
  }
  for (int row = 0; row < height_; row++) {
    delete[] energy_vals[row];
  }
  delete[] energy_vals;
  std::cout << "hi" << std::endl;
  return vertical_seam;
}

void SeamCarver::RemoveHorizontalSeam() {
  const int* horizontalseam = GetHorizontalSeam();
  image_.RemoveHorizontal(horizontalseam);
  height_ -= 1;
  delete[] horizontalseam;
}
void SeamCarver::RemoveVerticalSeam() {
  std::cout << "removing v" << std::endl;
  std::cout << height_ << " " << width_ << std::endl;
  const int* verticalseam = GetVerticalSeam();
  image_.RemoveVertical(verticalseam);
  width_ -= 1;
  delete[] verticalseam;
}
