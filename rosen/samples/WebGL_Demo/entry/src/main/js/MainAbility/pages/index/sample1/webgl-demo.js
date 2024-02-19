
//
// start here
//
export function main1(canvas) {
  let gl = canvas.getContext("webgl", {
    antialias: true, alpha: true, depth: true
  });
  // Only continue if WebGL is available and working
  if (gl === null) {
    console.log("gl is null")
    return;
  }

  // Set clear color to black, fully opaque
  gl.clearColor(0.0, 0.0, 0.0, 1.0);
  // Clear the color buffer with specified clear color
  gl.clear(gl.COLOR_BUFFER_BIT);
  gl.flush();
}
