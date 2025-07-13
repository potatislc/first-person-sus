uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

mat4 getMvp() {
    return u_projection * u_view * u_model;
}