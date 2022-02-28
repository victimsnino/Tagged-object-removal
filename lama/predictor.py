import sys
import os

sys.path.append(os.path.join(os.path.dirname(__file__), 'lama', 'bin'))
sys.path.append(os.path.join(os.path.dirname(__file__), 'lama'))

from predict import *

class Predictor:
    def create_network(self, weights_folder):
        # register_debug_signal_handlers()  # kill -10 <pid> will result in traceback dumped into log

        device = torch.device('cpu')

        train_config_path = os.path.join(weights_folder, 'config.yaml')
        with open(train_config_path, 'r') as f:
            train_config = OmegaConf.create(yaml.safe_load(f))
        
        train_config.training_model.predict_only = True
        train_config.visualizer.kind = 'noop'

        checkpoint_path = os.path.join(weights_folder, 
                                       'models', 
                                       'best.ckpt')
        model = load_checkpoint(train_config, checkpoint_path, strict=False, map_location='cpu')
        model.freeze()
        model.to(device)
        return model

    def __init__(self, weights_folder):
        self.model = self.create_network(weights_folder)
    
    def preprocess_img(self, img, mode='RGB', return_orig=False):
        img = np.array(cv2.cvtColor(img, mode))
        if img.ndim == 3:
            img = np.transpose(img, (2, 0, 1))
        else:
            img = np.expand_dims(img, axis = 0)
        out_img = img.astype('float32') / 255
        if return_orig:
            return out_img, img
        else:
            return out_img

    def ceil_modulo(self, x, mod):
        if x % mod == 0:
            return x
        return (x // mod + 1) * mod

    def pad_img_to_modulo(self, img, mod):
        channels, height, width = img.shape
        out_height = self.ceil_modulo(height, mod)
        out_width = self.ceil_modulo(width, mod)
        return np.pad(img, ((0, 0), (0, out_height - height), (0, out_width - width)), mode='symmetric')

    def predict(self, picture, mask):
        picture = self.preprocess_img(picture, mode = cv2.COLOR_BGR2RGB)
        picture = self.pad_img_to_modulo(picture, 8)
        print(picture.shape)
        mask = self.preprocess_img(mask, mode = cv2.COLOR_BGR2GRAY)
        mask = self.pad_img_to_modulo(mask, 8)
        print(picture.shape, mask.shape)
        with torch.no_grad():
            batch = move_to_device(default_collate([{
                'image' : picture,
                'mask' : mask
            }]), 'cpu')

            print(batch['mask'].shape)
            batch['mask'] = (batch['mask'] > 0) * 1
            prediction = self.model(batch)
            cur_res = prediction['inpainted'][0].permute(1, 2, 0).detach().cpu().numpy()
            cur_res = np.clip(cur_res * 255, 0, 255).astype('uint8')
            cur_res = cv2.cvtColor(cur_res, cv2.COLOR_RGB2BGR)
        return cur_res