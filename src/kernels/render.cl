kernel void render(cell_buffer_t cells, write_only image2d_t tex) {
    int x = get_global_id(0);
    int y = get_global_id(1);
    int w = get_global_size(0);

    write_imageui(tex, (int2)(x, y), (uint4)(cells[y * w + x], 0, 0, 0));
}
