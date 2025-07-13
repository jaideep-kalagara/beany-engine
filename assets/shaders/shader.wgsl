struct VertexInput {
    @location(0) pos: vec2f,
    @location(1) color: vec3f,
};

struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) fragColor: vec3f,
};

@vertex
fn vs_main(input: VertexInput) -> VertexOutput {
    var output: VertexOutput;

    // Manually normalize from [0.0, 1.5] to [-1.0, 1.0]
    let normalizedX = (input.pos.x / 1.5) * 2.0 - 1.0;
    let normalizedY = (input.pos.y / 1.0) * 2.0 - 1.0;

    output.position = vec4f(normalizedX, normalizedY, 0.0, 1.0);
    output.fragColor = input.color;

    return output;
}

@fragment
fn fs_main(input: VertexOutput) -> @location(0) vec4f {
    return vec4f(input.fragColor, 1.0);
}
