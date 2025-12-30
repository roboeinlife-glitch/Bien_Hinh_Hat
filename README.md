# Bien_Hinh_Hat
# 3D Particle Morph Visualizer

Một chương trình trực quan hóa 3D đẹp mắt sử dụng **SFML**, hiển thị các hình khối particle thay đổi hình dạng mượt mà. Các hình được tạo từ hàng nghìn hạt (particles) với màu sắc gradient, hiệu ứng độ sâu, trails, zoom, rotate, distort và chuyển đổi giữa các hình.

### Các hình hiện có
1. **Hollow Sphere** – Hình cầu rỗng nhiều lớp với lưới kết nối
2. **Hollow Cube** – Hình lập phương khung dây với hạt trên các cạnh và mặt
3. **Figure-8 Spiral** – Xoắn số 8 3D dạng khối dày đẹp mắt
4. **Atomic Model** – Mô hình nguyên tử với hạt nhân lung linh và các electron quay trên quỹ đạo nghiêng (rất đẹp!)
5. **3D Heart** – Trái tim 3D dạng khối với hạt bên trong
6. **Double Helix** – Xoắn kép giống DNA với bonds kết nối

### Tính năng nổi bật
- Xoay camera tự do bằng chuột
- Zoom mượt mà bằng bánh xe chuột
- Phóng to/thu nhỏ kích thước hình (Shift + Wheel)
- Biến dạng hình (Shift + Drag chuột trái)
- Chuyển đổi hình dạng bằng nút hoặc phím T
- Tự động xoay (Auto-rotate)
- Chu kỳ màu sắc (Color cycle)
- Hiệu ứng độ sâu và glow nhẹ cho particle
- Trails cho electron trong mô hình nguyên tử

### Điều khiển
| Phím / Hành động                  | Chức năng                              |
|-----------------------------------|----------------------------------------|
| `T` hoặc click nút "Transform Shape" | Chuyển sang hình tiếp theo             |
| Drag chuột trái                   | Xoay camera 3D                         |
| Bánh xe chuột                     | Zoom in/out camera                     |
| `Shift` + Bánh xe                 | Phóng to/thu nhỏ kích thước hình       |
| `Shift` + Drag chuột trái         | Biến dạng hình (distort)               |
| `Space`                           | Bật/tắt tự động xoay                   |
| `C`                               | Bật/tắt chu kỳ màu                     |
| `R`                               | Reset view (camera + scale + distort)  |
| `+` / `-`                         | Tăng/giảm kích thước particle          |
| `Page Up` / `Page Down`           | Scale hình nhanh                       |
| `Esc`                             | Thoát chương trình                     |

### Yêu cầu
- SFML 2.5 hoặc mới hơn
- Compiler hỗ trợ C++11 trở lên (g++ / clang / MSVC)
- Font `arial.ttf` (tùy chọn, nếu không có sẽ không hiển thị text)

### Cách build & chạy

#### Windows (Visual Studio, Code::Blocks hoặc MinGW)
bash
`g++ -O2 main.cpp -o ParticleMorph.exe -lsfml-graphics -lsfml-window -lsfml-system
./ParticleMorph.exe`

Chương trình được khởi tạo bởi ## DeepSeek AI 

Và hoàn thiện bởi ## Grok AI

[Trở về trang chủ](https://github.com/roboeinlife-glitch)


