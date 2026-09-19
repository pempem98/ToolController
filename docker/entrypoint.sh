#!/bin/bash
set -e

# Lấy UID và GID truyền vào từ môi trường (mặc định 1000 nếu không có)
USER_ID=${USER_ID:-1000}
GROUP_ID=${GROUP_ID:-1000}
USER_NAME="developer"

# Nếu chạy với UID=0 (root), thực thi trực tiếp lệnh
if [ "$USER_ID" -eq 0 ]; then
    exec "$@"
fi

# Kiểm tra xem group đã tồn tại chưa, nếu chưa thì tạo
if ! getent group "$GROUP_ID" > /dev/null 2>&1; then
    groupadd -g "$GROUP_ID" "$USER_NAME"
fi

# Kiểm tra xem user đã tồn tại chưa, nếu chưa thì tạo user với UID và GID tương ứng
if ! id -u "$USER_NAME" > /dev/null 2>&1; then
    useradd -u "$USER_ID" -g "$GROUP_ID" -m -s /bin/bash "$USER_NAME"
fi

# Chạy lệnh dưới danh tính của user vừa tạo thông qua gosu/su-exec
exec su "$USER_NAME" -c "$*"

