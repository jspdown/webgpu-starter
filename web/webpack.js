const path = require("path")
const assert = require("assert").strict
const HtmlWebpackPlugin = require("html-webpack-plugin")

module.exports = (env) => {
    assert(env.INPUT_DIR, "INPUT_DIR environment variable is not defined")
    assert(env.OUTPUT_DIR, "OUTPUT_DIR environment variable is not defined")
    assert(env.MODULE_DIR, "MODULE_DIR environment variable is not defined")

    const inputDir = env.INPUT_DIR
    const outputDir = env.OUTPUT_DIR
    const moduleDir = env.MODULE_DIR

    return {
        mode: "development",
        entry: {
            main: path.resolve(inputDir, "index.ts"),
        },
        output: {
            filename: "bundle.js",
            path: outputDir,
        },
        resolve: {
            extensions: [".ts", ".js"],
            modules: [
                moduleDir,
                "node_modules"
            ],
        },
        devtool: "source-map",
        module: {
            rules: [{
                test: /\.ts$/,
                use: "ts-loader",
                exclude: /node_modules/,
            },
            {
                test: /\.css$/,
                use: [
                    "style-loader",
                    {
                        loader: "css-loader",
                        options: {
                            url: true,
                            import: true,
                            modules: false,
                        },
                    },
                ],
            }],
        },
        plugins: [
            new HtmlWebpackPlugin({
                template: path.resolve(inputDir, "index.html"),
                filename: "index.html",
            }),
        ],
    }
}
