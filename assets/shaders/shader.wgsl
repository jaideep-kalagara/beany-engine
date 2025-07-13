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

    let ratio = 800.0 / 600.0;
    var offset = vec2f(-0.6875, -0.463);

    output.position = vec4f(input.pos.x + offset.x, (input.pos.y + offset.y) * ratio, 0.0, 1.0);
    output.fragColor = input.color;

    return output;
}

@fragment
fn fs_main(input: VertexOutput) -> @location(0) vec4f {
    let linear_color = pow(input.fragColor, vec3f(2.2));
	return vec4f(linear_color, 1.0);
}
