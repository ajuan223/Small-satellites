export default function verifyNumberInput(str: string, defaultValue?: number) {
    let num = parseInt(str);
    if (isNaN(num)) {
        num = defaultValue || 0;
    }
    return num.toString();
}
