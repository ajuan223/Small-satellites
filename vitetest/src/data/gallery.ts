import { ref } from "vue";

const gallery = ref<string[]>([]);
const currentPicture = ref("");

export { gallery, currentPicture };
